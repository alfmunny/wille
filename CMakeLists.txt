cmake_minimum_required(VERSION 3.10)
project(wille)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    wille/log.cpp
    wille/util.cpp
    )

add_library(wille SHARED ${LIB_SRC})

add_executable(test 
    tests/test.cpp
    )
add_dependencies(test wille)
target_link_libraries(test wille)
add_link_options(-rdynamic)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
