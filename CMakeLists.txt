cmake_minimum_required(VERSION 3.0.0)
project(sync-demo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(SOURCE_FILES
    src/Net/connection.h
    src/Net/connection.cc
    src/Net/netWork.h
    src/Net/network.cc
    src/Net/message.h
    src/Proto/msg.pb.h
    src/Proto/msg.pb.cc
    src/Game/player.h
    src/Game/player.cc
    src/Game/cube.h
    src/Game/cube.cc
    src/Game/world_manager.h
    src/Game/world_manager.cc
    src/Physics/physx_manager.h
    src/Physics/physx_manager.cc
    src/Physics/game_object.h
    src/Net/conn_manager.h
    src/Server/Server.h
    src/Server/Server.cc
    src/Proto/msg.pb.cc
    src/Proto/msg.pb.h
    src/Net/request.h
    src/Net/router.h
    src/Apis/init_player.h
    src/Apis/move.h
)

add_executable(sync-demo main.cpp ${SOURCE_FILES})

find_package(Boost REQUIRED system)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(sync-demo PRIVATE unofficial::omniverse-physx-sdk::sdk)
target_link_libraries(sync-demo PUBLIC Boost::system)
target_link_libraries(sync-demo PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
target_link_libraries(sync-demo PRIVATE spdlog::spdlog)