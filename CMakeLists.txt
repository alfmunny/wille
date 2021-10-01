cmake_minimum_required(VERSION 3.10)
project(wille)

include(cmake/util.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
set(BOOST_ROOT /opt/homebrew/Cellar/boost/1.76.0)
find_package(Boost)
find_library(PTHREAD pthread)

include_directories(.)
include_directories(~/usr/local/include)
link_directories(~/usr/local/lib)

set(LIB_SRC
    wille/log.cpp
    wille/util.cpp
    wille/config.cpp
    wille/mutex.h
    wille/thread.cpp
    )

add_library(wille SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(wille)


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

set(LIBS
    wille
    pthread)

add_executable(test_basic tests/test.cpp)
add_dependencies(test_basic wille)
target_link_libraries(test_basic ${LIBS})
force_redefine_file_macro_for_sources(test_basic)

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config wille)
target_link_libraries(test_config ${LIBS})
force_redefine_file_macro_for_sources(test_config)

add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread wille)
target_link_libraries(test_thread wille pthread)
force_redefine_file_macro_for_sources(test_thread)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_custom_target(run
    #COMMAND test_basic
    #COMMAND test_config
    COMMAND test_thread
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

#enable_testing()
#add_test(NAME MyTest COMMAND test_config)
