# for each "src/x.cpp", generate target "x"
file(GLOB_RECURSE all_chp5 *.cpp)
foreach(v ${all_chp5})
    string(REGEX MATCH "chp5/.*" relative_path ${v})
    # message(${relative_path})
    string(REGEX REPLACE "chp5/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})

    add_executable(${target_name} ${v})
endforeach()
