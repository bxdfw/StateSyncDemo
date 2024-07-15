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

void Player::init_player(){
    _physics = PhysxManager::Instance().create_new_player();

    if(!_physics){
        spdlog::error("PhysxManager::Instance().create_new_player() failed");
        return;
    }
    //todo sync pid
    Proto::SyncPid sync_pid;
    sync_pid.set_pid(_pid);
    size_t size = sync_pid.ByteSizeLong();
    std::vector<char> binary_array(size);
    if(sync_pid.SerializeToArray(binary_array.data(), size)){
            _conn->send(201,binary_array.data(), size);
    }else{
        spdlog::error("Player::init_player() serialized message id={} error", _pid);
    }
}

void Player::add_force(float horizontal, float vertical){
    _physics->addForce(physx::PxVec3({horizontal,0.0f,vertical}) * _speed);
}

physx::PxVec3 Player::position(){
    return _physics->getGlobalPose().p;
}

physx::PxVec4 Player::rotation(){
    auto q = _physics->getGlobalPose().q;
    return physx::PxVec4(q.x,q.y,q.z,q.w);
}

#endif //PLAYER_H