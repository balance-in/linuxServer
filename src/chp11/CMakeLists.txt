# for each "src/x.cpp", generate target "x"
file(GLOB_RECURSE all_chp11 *.cpp)
foreach(v ${all_chp11})
    string(REGEX MATCH "chp11/.*" relative_path ${v})
    # message(${relative_path})
    string(REGEX REPLACE "chp11/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})

    add_executable(${target_name} ${v})
    target_link_libraries(${target_name} ${CMAKE_THREAD_LIBS_INIT}) #添加linux多线程支持
endforeach()
