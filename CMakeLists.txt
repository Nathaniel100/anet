cmake_minimum_required(VERSION 3.6.0)
project(anet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(anet STATIC anet.cpp)

add_executable(anet_main anet_main.cpp)
target_link_libraries(anet_main anet)

add_executable(resolve_main resolve_main.cpp)
add_executable(echo_client echo_client.cpp)
add_executable(echo_server echo_server.cpp)
add_executable(printsockopt printsockopt.cpp)

add_executable(timeval_sizeof timeval_sizeof.cpp)
add_executable(echo_select_client echo_select_client.cpp)
target_link_libraries(echo_select_client anet)
add_executable(echo_select_server echo_select_server.cpp)
target_link_libraries(echo_select_server anet)
