#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <cmath>
#include "../Net/network.h"
#include "../Physics/physx_manager.h"
#include "spdlog/spdlog.h"
#include "../Proto/msg.pb.h"

class Player{
    friend class WorldManager;
public:
    Player(int pid, std::shared_ptr<Connection> conn, WorldManager& pm) 
        : _pid(pid), _conn(conn) { init_player(); }

    ~Player(){_physics->release();}

    void init_player();
    void add_force(float horizontal, float vertical);
    physx::PxVec3 position();
    physx::PxVec4 rotation();
private:
    int _pid;
    std::shared_ptr<Connection> _conn;
    
    //todo 其他需要同步的player属性
    physx::PxRigidDynamic* _physics;
    float _speed = 10.0f;
};

#endif //PLAYER_H