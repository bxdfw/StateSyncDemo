#ifndef NETWORK_H
#define NETWORK_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include "message.h"
#include "connection.h"
#include "spdlog/spdlog.h"
#include "conn_manager.h"
#include "request.h"

class NetworkManager{
    static const std::size_t kQueueCapacity = 128; 
public:
    NetworkManager(short port) 
        : _io_ctx(),_recv_queue(kQueueCapacity), _acceptor(_io_ctx, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),_conn_mgr(new ConnManager())
        {}

    void accept_thread();
    void read_clients_thread();
    void send_to_clients_thread();
    //void answer_to_client(); //todo 可改成update逻辑 一次update发送队列里的包

    std::vector<Request*> read();

    //void call_on_conn_start(std::function<void(std::shared_ptr<Connection>)> callback) { onConnStart = callback;}
    //void call_on_conn_stop(std::function<void(int)> callback) { onConnStop = callback;}
    std::shared_ptr<Connection> get_conn_by_pid(int pid);
private:
    boost::asio::io_context _io_ctx;
    boost::asio::ip::tcp::acceptor _acceptor;
    ConnManager* _conn_mgr;
    boost::shared_mutex _cs;

    boost::lockfree::spsc_queue<Request*> _recv_queue;
    //int _pcnt = 0;  //todo  玩家数 用于分配pid 暂时这么设计

    //std::function<void(std::shared_ptr<Connection>)> onConnStart;
    //std::function<void(int)> onConnStop;    //todo 玩家下线
};

#endif //NETWORK_H