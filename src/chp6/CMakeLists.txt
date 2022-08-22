# for each "src/x.cpp", generate target "x"
file(GLOB_RECURSE all_chp6 *.cpp)
foreach(v ${all_chp6})
    string(REGEX MATCH "chp6/.*" relative_path ${v})
    # message(${relative_path})
    string(REGEX REPLACE "chp6/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})

    add_executable(${target_name} ${v})
endforeach()
