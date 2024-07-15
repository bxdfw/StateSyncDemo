#ifndef MESSAGE_H
#define MESSAGE_H

#include <memory>

class Message{
public:
    static const size_t kHeadLen = sizeof(int32_t) + sizeof(size_t);

    Message(int32_t id, const char data[], size_t n): _id(id) {
        _data_len = n;
        _data = new char[_data_len];
        std::memcpy(_data, data, _data_len);
    }

    Message& operator=(const Message& other) {
        if (this == &other) {
            return *this;
        }
        delete[] _data;

        _id = other._id;
        _data_len = other._data_len;
        _data = new char[_data_len];
        std::memcpy(_data, other._data, _data_len);

        return *this;
    }

    ~Message(){delete[] _data;}

    void pack(char* buffer) const {
        std::memcpy(buffer, &_id, sizeof(_id));
        std::memcpy(buffer + sizeof(_id), &_data_len, sizeof(_data_len));
        std::memcpy(buffer + sizeof(_id) + sizeof(_data_len), _data, _data_len);
    }

    static Message* unpack(const char* buffer, size_t size) {
        if (size < sizeof(int32_t) + sizeof(size_t)) {
            // 缓冲区大小不足以容纳消息头
            return nullptr;
        }

        int32_t id;
        size_t data_len;
        std::memcpy(&id, buffer, sizeof(id));
        std::memcpy(&data_len, buffer + sizeof(id), sizeof(data_len));

        if (size < sizeof(id) + sizeof(data_len) + data_len) {
            // 数据部分不完整
            return nullptr;
        }
        const char* data = buffer + sizeof(id) + sizeof(data_len);
        return new Message(id, data, data_len);
    }
    
    int32_t id() const {return _id;}
    char* data() const {return _data;}
    size_t data_len() const { return _data_len;}
private:
    int32_t _id;
    size_t _data_len;
    char* _data;
};

#endif //MESSAGE_H