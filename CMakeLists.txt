cmake_minimum_required(VERSION 3.10)
project(etn_hw)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server_main.cpp Device.h NetworkTools.h Client.h Server.cpp Server.h)
add_executable(client client_main.cpp Device.cpp Device.h NetworkTools.cpp NetworkTools.h TempMonitor.cpp TempMonitor.h UptimeMonitor.cpp UptimeMonitor.h Client.cpp Client.h)