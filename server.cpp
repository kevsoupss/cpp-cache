#include "server.h"
#include <iostream>
#include <vector>
#include <stdexcept>

#pragma comment(lib, "Ws2_32.lib")

Server::Server(unsigned short port, CommandHandler handler)
    : port_(port), handler_(std::move(handler)), listenSocket_(INVALID_SOCKET), running_(false) {
}

bool Server::start() {
    if (!initWinsock()) return false;
    if (!bindAndListen()) return false;

    pollFds_.push_back({listenSocket_, POLLRDNORM, 0});
    running_ = true;

    // Single thread
    while (running_) {
        // Blocks until client connection
        int ret = WSAPoll(pollFds_.data(), pollFds_.size(), -1);
        if (ret <= 0) continue;

        for (size_t i = 0; i < pollFds_.size(); ++i) {
            if (pollFds_[i].revents == 0) continue;

            if (pollFds_[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                removeClient(i);
                continue;
            }

            // New connection on listening socket or handle client data
            if (pollFds_[i].fd == listenSocket_) {
                acceptNewClient();
            } else {
                handleClientData(i);
            }
        }
    }
    cleanup();
    return true;
}

void Server::stop() {
    running_ = false;
    if (listenSocket_ != INVALID_SOCKET) {
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }
    WSACleanup();
}

bool Server::initWinsock() {
    // Initialize Winsock
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
        std::cerr << "WSAStartup failed: " << res << "\n";
        return false;
    }
    return true;
}

bool Server::bindAndListen() {
    // Create server socket
    listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket_ == INVALID_SOCKET) {
        std::cerr << "socket() failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return false;
    }

    // Socket options
    BOOL opt = TRUE;
    setsockopt(listenSocket_, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *) &opt, sizeof(opt));

    // Address structure
    sockaddr_in service{};
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port_);

    // Bind socket to port and address
    if (bind(listenSocket_, (sockaddr *) &service, sizeof(service)) == SOCKET_ERROR) {
        std::cerr << "bind() failed: " << WSAGetLastError() << "\n";
        closesocket(listenSocket_);
        WSACleanup();
        return false;
    }

    // Server starts listening
    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen() failed: " << WSAGetLastError() << "\n";
        closesocket(listenSocket_);
        WSACleanup();
        return false;
    }

    std::cout << "RedisLite listening on port " << port_ << "...\n";
    return true;
}

void Server::removeClient(size_t &idx) {
    SOCKET removedSocket = pollFds_[idx].fd;
    closesocket(removedSocket);
    pollFds_.erase(pollFds_.begin() + idx);
    clientSessions_.erase(removedSocket);
    --idx;
    //std::cout << "Client disconnected (Error/HUP).\n";
}

void Server::acceptNewClient() {
    SOCKET clientSocket = accept(listenSocket_, nullptr, nullptr);
    if (clientSocket != INVALID_SOCKET) {
        WSAPOLLFD clientFd = {};
        clientFd.fd = clientSocket;
        clientFd.events = POLLRDNORM;
        pollFds_.push_back(clientFd);
        clientSessions_[clientSocket] = {clientSocket, ""};
        //std::cout << "New client connected.\n";
    }
}

void Server::handleClientData(size_t& i) {
    ClientSession& session = clientSessions_[pollFds_[i].fd];
    SOCKET& clientSocket = session.socket;
    std::string& readBuffer = session.buffer;

    if (pollFds_[i].revents & POLLRDNORM) {
        int BUFFER_SZ = 4096;
        static std::vector<char> tmp(BUFFER_SZ);

        int bytes = recv(clientSocket, tmp.data(), BUFFER_SZ, 0);

        if (bytes <= 0) {
           removeClient(i);
        } else {
            readBuffer.append(tmp.data(), bytes);
            size_t pos = 0;
            while (pos < readBuffer.size()) {
                size_t lastPos = pos;
                try {
                    RespValue req = parseValue(readBuffer, pos);

                    // Reply from handler
                    RespValue reply = handler_(req);

                    // Serialize and send back
                    std::string out = serialize(reply);
                    int sent = send(clientSocket, out.c_str(), (int) out.size(), 0);
                    if (sent == SOCKET_ERROR) {
                        std::cerr << "send() failed: " << WSAGetLastError() << "\n";
                        break;
                    }
                } catch (const IncompleteMessageException) {
                    pos = lastPos;
                    break;
                } catch (const std::runtime_error &e) {
                    handleProtocolError(clientSocket, e.what());
                    removeClient(i);
                    return;
                }
            }
            if (pos > 0) {
                readBuffer.erase(0, pos);
            }
        }
    }
}

void Server::handleProtocolError(SOCKET s, const std::string& errorMessage) {
    std::cerr << "Protocol Error on socket " << s << ": " << errorMessage << "\n";

    std::string respError = "-ERR Protocol Error: " + errorMessage + "\r\n";

    send(s, respError.c_str(), static_cast<int>(respError.size()), 0);
}

void Server::cleanup() {
    closesocket(listenSocket_);
    WSACleanup();
}