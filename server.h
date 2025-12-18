#ifndef CACHE_SERVER_H
#define CACHE_SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <functional>
#include <string>
#include "parser.h"
#include "serializer.h"
#include "resp.h"

class Server {
public:
    using CommandHandler = std::function<RespValue(const RespValue&)>;
    Server(unsigned short port, CommandHandler handler);

    bool start();
    void stop();

private:
    struct ClientSession {
        SOCKET socket;
        std::string buffer;
    };

    std::unordered_map<SOCKET, ClientSession> clientSessions_;
    std::vector<WSAPOLLFD> pollFds_;
    CommandHandler handler_;
    SOCKET listenSocket_;
    unsigned short port_;
    bool running_;

    bool initWinsock();
    bool bindAndListen();
    void removeClient(size_t&);
    void acceptNewClient();
    void handleClientData(size_t&);
    void handleProtocolError(SOCKET, const std::string&);
    void cleanup();
};


#endif //CACHE_SERVER_H