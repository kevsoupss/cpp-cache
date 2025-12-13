//
// Created by kevin on 12/11/2025.
//

#ifndef CACHE_HANDLER_H
#define CACHE_HANDLER_H
#include "resp.h"


class Handler {
public:
    static RespValue handler(const RespValue& req);

private:
    static RespValue handleGet(const RespValue& value);
};


#endif //CACHE_HANDLER_H