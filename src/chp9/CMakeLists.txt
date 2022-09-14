# for each "src/x.cpp", generate target "x"
file(GLOB_RECURSE all_chp9 *.cpp)
foreach(v ${all_chp9})
    string(REGEX MATCH "chp9/.*" relative_path ${v})
    # message(${relative_path})
    string(REGEX REPLACE "chp9/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})

    add_executable(${target_name} ${v})
    target_link_libraries(${target_name} ${CMAKE_THREAD_LIBS_INIT}) #添加linux多线程支持
endforeach()
