cmake_minimum_required(VERSION 3.29)
project(Messenger)

set(CMAKE_CXX_STANDARD 17)

include_directories("D:/CppLibrary/asio-1.30.2/include")

add_executable(ClientMessenger main.cpp)

target_link_libraries(ClientMessenger ws2_32)