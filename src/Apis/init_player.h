#include "../Net/router.h"
#include "../Proto/msg.pb.h"
#include "../Game/world_manager.h"

class InitPlayerApi : public Router {
public:
    void handle(Request* req ) override {
        //spdlog::debug("handle message id=1");
        WorldManager::instance().add_player(req->get_connection());
    }
};