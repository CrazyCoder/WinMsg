cmake_minimum_required(VERSION 3.28)
project(winmsg)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ -static")

add_executable(winmsg WIN32 src/main.cpp src/resource.h)
target_sources(winmsg PRIVATE "res/winmsg.rc")
