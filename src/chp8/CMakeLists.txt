# for each "src/x.cpp", generate target "x"
file(GLOB_RECURSE all_chp8 *.cpp)
foreach(v ${all_chp8})
    string(REGEX MATCH "chp8/.*" relative_path ${v})
    # message(${relative_path})
    string(REGEX REPLACE "chp8/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})

    add_executable(${target_name} ${v})
endforeach()
