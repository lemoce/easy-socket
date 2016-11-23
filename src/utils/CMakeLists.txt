include_directories ("${PROJECT_SOURCE_DIR}/src/lib")

add_executable(arch arch.cpp)

add_executable(http_get http_get.cpp)
target_link_libraries(http_get socket)
set_property(TARGET http_get PROPERTY CXX_STANDARD 11)

add_executable(daytime_server daytime_server.cpp)
target_link_libraries(daytime_server socket)
set_property(TARGET daytime_server PROPERTY CXX_STANDARD 11)

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server socket ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET echo_server PROPERTY CXX_STANDARD 11)

add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client socket)
set_property(TARGET echo_client PROPERTY CXX_STANDARD 11)

add_executable(tcptunnel tcptunnel.cpp)
target_link_libraries(tcptunnel socket ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET tcptunnel PROPERTY CXX_STANDARD 11)
