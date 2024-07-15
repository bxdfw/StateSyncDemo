#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <memory>
#include "connection.h"
#include <spdlog/spdlog.h>

class ConnManager{
public:
    void add(std::shared_ptr<Connection> conn){
        _conn_id++;
        _conns.emplace(_conn_id,conn);
        spdlog::info("new client connected, count of connections is {}", _conns.size());
    }

    std::shared_ptr<Connection> get(int id){
        auto it = _conns.find(id);
        if(it == _conns.end()){
            spdlog::warn("can not find connection id={}",id);
            return std::shared_ptr<Connection>();
        }
        return it->second;
    }
    
    void remove(int id){
        auto it = _conns.find(id);
        if(it != _conns.end()){
            it->second->stop();
            _conns.erase(id);
        }else{
            spdlog::warn("connection id={} alread removed",id);
        }
    }

    const std::unordered_map<int, std::shared_ptr<Connection> >& conns() const { return _conns;}
private:
    std::unordered_map<int, std::shared_ptr<Connection> > _conns;
    int _conn_id = 0;
};