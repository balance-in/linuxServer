cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("Setting default build type to Release")
endif()

project(my_project_name VERSION 0.0.1 LANGUAGES C CXX)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(Threads) #添加linux多线程支持

# add_subdirectory(./src/chp5)

add_subdirectory(./src/chp10)

add_subdirectory(./test)

add_subdirectory(./example)
