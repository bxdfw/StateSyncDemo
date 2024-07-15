#ifndef ROUTER_H
#define ROUTER_H

#include "request.h"

class Router {
public:
    virtual void handle(Request* req) = 0;
    virtual ~Router(){ }
};

#endif //ROUTER_h