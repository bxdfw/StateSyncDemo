#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
#include <boost/asio.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <queue>
#include "message.h"
#include "spdlog/spdlog.h"

class Connection : std::enable_shared_from_this<Connection>{
    static const std::size_t kQueueCapacity = 128; 
public:
    explicit Connection(boost::asio::ip::tcp::socket& sock) 
        : _socket(std::move(sock)), _send_queue(kQueueCapacity) {}

    boost::asio::ip::tcp::socket& sock() { return _socket;}

    void send(int id, const char data[], size_t n);
    Message* answer_to_client();
    bool send_to_client();
    void stop();
    bool is_socket_open(){ return _socket.is_open();}
    //void recv();
private:
    //bool timed_out() const;   //todo 客户端长期不动掉线 + 断线重连

    void read_request();
    Message*  process_request();

    //todo 析构函数 析构时queue里还有msg 可能会造成泄露？
private:
    boost::asio::ip::tcp::socket _socket;
    //boost::posix_time::ptime last_ping;

    enum {max_packet_len = 1024};
    char _read_buffer[max_packet_len];
    int  _already_read = 0;
    boost::lockfree::spsc_queue<Message*> _send_queue;
};

#endif //CONNECTION_H