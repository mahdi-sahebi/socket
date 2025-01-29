cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(socket LANGUAGES CXX VERSION 1.0)
# TODO(MN): Rename to socket. Add TCP, ICMP, Large scale sockets

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
