#include"src/Server/server.h"
#include <boost/asio.hpp>
#include <memory>


int main(){
    spdlog::set_level(spdlog::level::trace);
    Server server(8080);
    server.start();
}