cmake_minimum_required(VERSION 3.3)
project(isa)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp SNMPClient.cpp SNMPClient.h SNMPObject.cpp SNMPObject.h SNMPPacket.cpp SNMPPacket.h)
add_executable(isa ${SOURCE_FILES})

if(WIN32)
    target_link_libraries(isa wsock32 ws2_32)
endif()