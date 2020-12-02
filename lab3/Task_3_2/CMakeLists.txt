cmake_minimum_required(VERSION 3.17)
project(Task_3_2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

find_package(OpenMP REQUIRED)
add_executable(Task_3_2 main.cpp)
target_link_libraries(Task_3_2 PRIVATE OpenMP::OpenMP_CXX)