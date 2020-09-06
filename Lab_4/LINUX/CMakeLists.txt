cmake_minimum_required(VERSION 3.16)
project(Lab_4)

set(CMAKE_CXX_STANDARD 20)

add_executable(Lab_4 main.cpp)

find_package(Threads REQUIRED)
target_link_libraries(Lab_4 PRIVATE Threads::Threads)