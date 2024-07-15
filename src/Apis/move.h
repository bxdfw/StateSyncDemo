#include "../Net/router.h"
#include "../Proto/msg.pb.h"
#include "../Game/world_manager.h"
#include <spdlog/spdlog.h>

class MoveApi : public Router {
public:
    void handle(Request* req ) override {
        //spdlog::debug("handle message id=2");
        Proto::MoveSphere move;
        bool ok = move.ParseFromArray(req->get_message()->data(),req->get_data_len());
        if(!ok){
            spdlog::error("can not parse Proto::MoveSphere from array");
            return;
        }

        Player* player = WorldManager::instance().get_player_by_pid(move.pid());
        if(!player){
            spdlog::error("can not find player pid={}", move.pid());
            return;
        }

        player->add_force(move.horizontal(),move.vertical());
    }
};
