cmake_minimum_required(VERSION 3.10)
project(wille)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.76.0)
find_package(Boost)

include_directories(.)
include_directories(~/usr/local/include)
link_directories(~/usr/local/lib)

set(LIB_SRC
    wille/log.cpp
    wille/util.cpp
    wille/config.cpp
    )

add_library(wille SHARED ${LIB_SRC})

if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${BOOST_ROOT})
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

find_library(YAMLCPP yaml-cpp)
target_link_libraries(${PROJECT_NAME} ${YAMLCPP})

add_executable(test tests/test.cpp)
add_dependencies(test wille)
target_link_libraries(test wille)

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config wille)
target_link_libraries(test_config wille)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
