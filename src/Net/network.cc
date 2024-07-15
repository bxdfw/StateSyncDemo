#include "network.h"

void NetworkManager::accept_thread() {
    spdlog::info("accept thread start ... listening at {}:{}", _acceptor.local_endpoint().address().to_string(), _acceptor.local_endpoint().port());
    while (true) {
        boost::asio::ip::tcp::socket socket(_io_ctx);
        auto conn_ptr = std::make_shared<Connection>(socket);
        _acceptor.accept(conn_ptr->sock());
        
        _cs.lock();
        _conn_mgr->add(conn_ptr);
        _cs.unlock();
    }
}

void NetworkManager::read_clients_thread(){
    spdlog::info("read thread start ...");
    while (true) {
        std::vector<int> to_remove;
        //std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 留出时间让accept_thread接收新连接 用读写锁应该不用了
        _cs.lock_shared();
        
        for(auto [n,v] : _conn_mgr->conns()){
            // todo 删除已经超时的客户端
            Message* msg = v->answer_to_client();
            if(msg == nullptr)
                continue;
            else
                _recv_queue.push(new Request(v,msg));
            
        }
        _cs.unlock_shared();
    }
}

void NetworkManager::send_to_clients_thread(){
    spdlog::info("send thread start ...");
    while(true){
        std::vector<int> to_remove;
        _cs.lock_shared();
        for(auto [n,v] : _conn_mgr->conns()){
            if(!v->send_to_client()){
                to_remove.push_back(n);
            }
        }
        _cs.unlock_shared();     

        // todo 处理关闭的连接， send时的异常处理
        if(!to_remove.empty()){
            _cs.lock();
            for(int v : to_remove){
                spdlog::info("client id={} offline", v);
                _conn_mgr->remove(v);
            }
            _cs.unlock();
        }     
    }
}

std::shared_ptr<Connection> NetworkManager::get_conn_by_pid(int pid){
    _cs.lock_shared();
    auto p = _conn_mgr->get(pid);
    _cs.unlock_shared();    
    return p;
}

//尽可能多的从recv_queue中取request,使用后记得delete request
std::vector<Request*> NetworkManager::read(){
    std::vector<Request*> requests;

    _recv_queue.consume_all([&requests](Request* req) {
            requests.push_back(req);
    });
    return requests;
}

//todo send read 错误日志 pid not found
// void NetworkManager::send(int pid, Message* msg){
//     auto conn = get_conn_by_pid(pid);
//     if(conn){
//        // conn->send(msg);
//     }
// }

// // todo 从recv队列获取一个包 队列为空返回nullptr 使用完这个包应该delete
// Message* NetworkManager::read(int pid){
//     auto conn = get_conn_by_pid(pid);
//     if(conn){
//         //return conn->read();
//     }

//     return nullptr;
// }