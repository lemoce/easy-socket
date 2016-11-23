find_package (Threads REQUIRED)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0600 ")
    option ("USE_WINSOCK" "Usa winsock" "ON")
    if (${MINGW})
        SET(BUILD_SHARED_LIBRARIES OFF)
        SET(CMAKE_EXE_LINKER_FLAGS "-static")
    endif()
endif()

configure_file ("config/config.h.in" "${PROJECT_BINARY_DIR}/config.h")

include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("lib")

add_subdirectory ("utils")
add_subdirectory ("lib")

