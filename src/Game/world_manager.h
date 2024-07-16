#ifndef WORLD_MANAGER
#define WORLD_MANAGER

#include <map>
#include <memory>
#include "player.h"
#include "cube.h"
#include "../Net/connection.h"
#include "spdlog/spdlog.h"

class WorldManager{
public:
   static WorldManager& instance() {
        static WorldManager instance;
        return instance;
    }
    
    void add_player(std::shared_ptr<Connection>);  //todo 注册player 删除player
    Player* get_player_by_pid(int pid);
    void sync_all(int64_t frame);
    void broadcast(int id, const char* data, size_t n);
    void remove_player(int pid);
    void create_cubes();
private:
     WorldManager() { }
    ~WorldManager() { } // todo 析构函数 释放player资源

    std::map<int, Player*> _players;
    int _pcnt = 0;  //_pcnt 分配 player id
    
    std::map<int, Cube*> _cubes;
    int _ccnt = 0;
};

#endif //WORLD_MANAGER