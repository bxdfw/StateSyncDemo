#ifndef REQUEST_H
#define REQUEST_H

#include "connection.h"
#include "message.h"
#include <memory>

class Request{
public:
    Request(std::shared_ptr<Connection> conn, Message* message):_conn(conn), _msg(message){ }
    ~Request(){ delete _msg;}

    std::shared_ptr<Connection> get_connection(){ return _conn;}
    Message* get_message(){return _msg;}
    int get_id(){return _msg->id();}
    int get_data_len(){return _msg->data_len();}
    void get_data(char* data){memcpy(data,_msg->data(),_msg->data_len());}

private:
    std::shared_ptr<Connection> _conn;
    Message* _msg;
};

#endif //REQUEST_H