cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

set(LINK_EVENT /usr/lib/libevent/lib)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("Setting default build type to Release")
endif()

project(my_project_name VERSION 0.0.1 LANGUAGES C CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(Threads) #添加linux多线程支持

link_directories(${LINK_EVENT})

add_subdirectory(./src/chp15)

add_subdirectory(./src/chp16)


