cmake_minimum_required(VERSION 3.21)
project(BotKrok)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

add_executable(BotKrok main.cpp SocketSession.cpp SocketSession.h MsgProcess.cpp MsgProcess.h Bot.cpp Bot.h)
add_executable(test test_server.cpp)