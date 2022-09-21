# for each "src/x.cpp", generate target "x"
file(GLOB_RECURSE all_chp14 *.cpp)
foreach(v ${all_chp14})
    string(REGEX MATCH "chp15/.*" relative_path ${v})
    # message(${relative_path})
    string(REGEX REPLACE "chp15/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})

    add_executable(${target_name} ${v})
    target_link_libraries(${target_name} ${CMAKE_THREAD_LIBS_INIT}) #添加linux多线程支持
    TARGET_LINK_LIBRARIES(${target_name} ${Boost_LIBRARIES} rt) #添加对共享内存支持
endforeach()
