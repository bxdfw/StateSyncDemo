#include "world_manager.h"

Player* WorldManager::get_player_by_pid(int pid){
    auto it = _players.find(pid);
    return it != _players.end() ? it->second : nullptr;
}

void WorldManager::remove_player(int pid){
    auto it = _players.find(pid);
    if(it != _players.end()){
        delete it->second;
        _players.erase(it);
    }
}

void WorldManager::add_player(std::shared_ptr<Connection> conn){
    _pcnt++;
    _players.emplace(_pcnt,new Player(_pcnt, conn, *this));
    spdlog::debug("add new player pid={}", _pcnt);
}

void WorldManager::broadcast(int id, const char* data, size_t n){
    for(auto p : _players){
        p.second->_conn->send(id,data,n);
    }
}

void WorldManager::sync_all(int64_t frame){
    //spdlog::debug("sync all object frame number={}",frame);
    Proto::Sync all;
    all.set_frame(frame);
    
    std::vector<int> to_remove;
    for(auto p : _players){
        if(!p.second->_conn->is_socket_open()){
            to_remove.push_back(p.first);
            continue;
        }

        auto player = all.add_players();
        player->set_pid(p.second->_pid);
        Proto::Transform* transform = new Proto::Transform();
        Proto::Position* position = new Proto::Position();
        Proto::Rotation* rotation = new Proto::Rotation();

        auto vec3 = p.second->position();
        position->set_x(vec3.x);
        position->set_y(vec3.y);
        position->set_z(vec3.z);

        auto vec4 = p.second->rotation();
        rotation->set_x(vec4.x);
        rotation->set_y(vec4.y);
        rotation->set_z(vec4.z);
        rotation->set_w(vec4.w);

        transform->set_allocated_position(position);
        transform->set_allocated_rotation(rotation);
        player->set_allocated_transform(transform);
    }

    for(int i : to_remove){
        remove_player(i);
    }

    //todo 限制一次同步的方块数量 可能会造成一个包过大
    for(auto c : _cubes){
        auto cube = all.add_cubes();
        cube->set_cid(c.second->id());

        Proto::Transform* transform = new Proto::Transform();
        Proto::Position* position = new Proto::Position();
        Proto::Rotation* rotation = new Proto::Rotation();

        auto vec3 = c.second->position();
        position->set_x(vec3.x);
        position->set_y(vec3.y);
        position->set_z(vec3.z);

        auto vec4 = c.second->rotation();
        rotation->set_x(vec4.x);
        rotation->set_y(vec4.y);
        rotation->set_z(vec4.z);
        rotation->set_w(vec4.w);

        transform->set_allocated_position(position);
        transform->set_allocated_rotation(rotation);
        cube->set_allocated_transform(transform);
    }

    size_t size = all.ByteSizeLong();
    std::vector<char> binary_array(size);
    if(all.SerializeToArray(binary_array.data(), size)){
        broadcast(203,binary_array.data(), size);
    }else{
        spdlog::error("PlayerManager::sync_all() serialized message id=203 error");
    }
}

void WorldManager::create_cubes(){
    std::vector<physx::PxVec3> position;

    int count = 0;
    physx::PxVec3 dimensions(0.6f, 0.6f, 0.6f);
    for (float y = 0.0f; y < 5.0f && count < 100; y += dimensions.y) {
        for (float z = 0.5f; z < 10.0f && count < 1000; z += dimensions.z) {
            for (float x = 0.5f; x < 10.0f && count < 1000; x += dimensions.x) {
                position.emplace_back(physx::PxVec3(x, y + 0.5f, z));
                count++;
            }
        }
    }

    for(physx::PxVec3& pos : position){
        ++_ccnt;
        Cube* cube = new Cube(_ccnt,pos);
        _cubes.emplace(cube->id(),cube);
    }
}
