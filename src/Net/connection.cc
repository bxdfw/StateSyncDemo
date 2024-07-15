#include "connection.h"

void Connection::send(int id, const char data[], size_t n){
    //todo push返回false  队列满 处理异常 日志
    Message* msg = new Message(id,data,n);
    _send_queue.push(msg);
}

bool Connection::send_to_client(){
    //todo write
    //todo 一次写1024bytes？
    const size_t buffer_size = 1024 * 512;
    char write_buffer[buffer_size];
    size_t already_write = 0;

    while(!_send_queue.empty()){
        auto msg = _send_queue.front();
        if (buffer_size - already_write < msg->kHeadLen + msg->data_len()){
            if(already_write == 0)
                spdlog::error("Connection::send_to_client() buffer_size={} too small", buffer_size);
            break;
        }
        msg->pack(write_buffer + already_write);
        already_write += msg->kHeadLen + msg->data_len();
        _send_queue.pop();
        delete msg;
    }

    boost::system::error_code err;
    _socket.write_some(boost::asio::buffer(write_buffer,already_write),err);
    if(err){
        spdlog::error("socket write error: {}", err.to_string());
        return false;
    }

    return true;
}

void Connection::stop() {
    boost::system::error_code err;
    _socket.close(err);
}

void Connection::read_request(){
    if(_socket.available()){
        boost::system::error_code err;
        _already_read += _socket.read_some(boost::asio::buffer(_read_buffer + _already_read, max_packet_len - _already_read),err);
        if(err){
            spdlog::error("socket read error: {}", err.to_string());
        }
    }
}

Message* Connection::process_request(){
    Message* message = Message::unpack(_read_buffer, _already_read); //todo 记得 delete
    if(!message){
        return nullptr;
    }

    std::copy(_read_buffer + Message::kHeadLen + message->data_len(), _read_buffer + _already_read, _read_buffer);
    _already_read -=  Message::kHeadLen + message->data_len();
    return message;
}

Message* Connection::answer_to_client(){
    try {
        read_request();
        return process_request();
    } catch ( boost::system::system_error&) { 
        spdlog::error("answer_to_client error");
        return nullptr;
    }
        //todo 超时断开连接
        //if ( timed_out())
        //    stop();
}