project(parser)

cmake_minimum_required(VERSION 2.6)

if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
    message(WARNING "No build type selected. Using 'Release' by default.")
endif()

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wl,--no-undefined -fPIC -std=gnu99 -Wall -Wconversion -Werror=implicit-function-declaration")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wl,--no-undefined -fPIC -Wall -Wconversion -Werror=implicit-function-declaration")

add_subdirectory("modules/markdowntown")
add_subdirectory("tools/markdowntown")
