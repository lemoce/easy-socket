find_package (Catch REQUIRED)

include_directories (${CATCH_INCLUDE_DIRS})

add_executable (main main.cpp)
set_property(TARGET main PROPERTY CXX_STANDARD 11)

