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

void Server::start(){
    //todo 断开连接时删除用户
    //_net.call_on_conn_start([this](std::shared_ptr<Connection> conn){PlayerManager::instance().add_player(conn);});

    _apis.emplace(1, std::move(std::make_unique<InitPlayerApi>()));
    _apis.emplace(2, std::move(std::make_unique<MoveApi>()));

    std::thread thread_accept([this]{_net.accept_thread();});
    std::thread thread_read  ([this]{_net.read_clients_thread();});
    std::thread thread_write ([this]{_net.send_to_clients_thread();});
    
    auto& physics = PhysxManager::Instance();
    physics.initialize();

    auto& players = WorldManager::instance();
    WorldManager::instance().create_cubes();

    const int target_fps = 60;
    float step  = 1.0f / target_fps;
    const std::chrono::milliseconds frame_duration(1000 / target_fps);
    std::chrono::duration<double, std::milli> frame_time(1000 / target_fps);

    int64_t frame_num = 1;
    std::chrono::duration<double, std::milli> frame_time_cnt(0);
    int frame_num_per_second = 0;
    while(true){
        auto frame_start = std::chrono::steady_clock::now();
        // 处理每帧的逻辑
        //process_frame_logic();
        physics.update(step);
        auto reqs = _net.read();
        for(Request* req : reqs){
            auto api = _apis.find(req->get_id());
            if(api == _apis.end()){
                spdlog::error("can not find handler message id={}",req->get_id());
                continue;
            }

            api->second->handle(req);
            delete req;
        }

        //todo 每6帧发送一个包同步数据
        //if(frame_num % 6 == 0){
            players.sync_all(frame_num);
        //}

        auto frame_end = std::chrono::steady_clock::now();
        frame_time = frame_end - frame_start;
        frame_num++;
        
        frame_time_cnt += frame_time;
        frame_num_per_second++;

        // 如果帧处理的时间少于目标时间，则睡眠剩余的时间
        if (frame_time < frame_duration) {
            std::this_thread::sleep_for(frame_duration - frame_time);
        } else {
             //spdlog::info("low Frames Per Second : {}", frame_time.count());
            // todo 如果帧处理时间超过目标时间，可以选择打印警告或日志，或调整逻辑
        }

        
        if(frame_time_cnt.count() >= 1000){
            spdlog::info("Frames Per Second : {}", frame_num_per_second);
            frame_num_per_second=0;
            frame_time_cnt = std::chrono::duration<double, std::milli>(0.0);
        }

        //todo 不限制服务器帧数，  限制发包频率
    }

    thread_accept.join();
    thread_read.join();
    thread_write.join();

    spdlog::trace("update");
}