#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <map>
#include "../Net/network.h"
#include "../Game/world_manager.h"
#include "../Physics/physx_manager.h"
#include "../Net/router.h"
#include "../Apis/init_player.h"
#include "../Apis/move.h"

class Server {
public:
    Server(short port): _net(port){ }
    void start();
private:
    NetworkManager _net;
    std::map<int,std::unique_ptr<Router>> _apis;
};

#endif //SERVER_H