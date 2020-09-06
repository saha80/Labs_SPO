cmake_minimum_required(VERSION 3.16)
project(Lab_5)

set(CMAKE_CXX_STANDARD 20)

add_library(a_in_out SHARED a_in_out.cpp)

target_include_directories(a_in_out PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

target_link_libraries(${PROJECT_NAME} PRIVATE a_in_out)