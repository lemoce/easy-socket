cmake_minimum_required (VERSION 3.0)
project (socket CXX)

include (CheckFunctionExists)
check_function_exists (usleep USE_POSIX)

add_subdirectory (src)
add_subdirectory (test)

# get_cmake_property(_variableNames VARIABLES)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
