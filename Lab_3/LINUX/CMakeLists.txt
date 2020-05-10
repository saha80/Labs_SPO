cmake_minimum_required(VERSION 3.16)
project(Lab_3)


set(CMAKE_CXX_STANDARD 20)

add_executable(server server/server.cpp)
add_executable(client client/client.cpp)
