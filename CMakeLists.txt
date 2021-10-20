cmake_minimum_required(VERSION 3.10)
project(wille)

include(cmake/util.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++14 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
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
    wille/mutex.cpp
    wille/thread.cpp
    wille/fiber.cpp
    wille/scheduler.cpp
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
target_link_libraries(test_thread ${LIBS})
force_redefine_file_macro_for_sources(test_thread)

add_executable(test_util tests/test_util.cpp)
add_dependencies(test_util wille)
target_link_libraries(test_util ${LIBS})
force_redefine_file_macro_for_sources(test_util)

add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber wille)
target_link_libraries(test_fiber ${LIBS})
force_redefine_file_macro_for_sources(test_fiber)

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler wille)
target_link_libraries(test_scheduler ${LIBS})
force_redefine_file_macro_for_sources(test_scheduler)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_custom_target(run
    #COMMAND test_basic
    #COMMAND test_config
    #COMMAND test_thread
    #COMMAND test_util
    COMMAND test_fiber
    COMMAND test_scheduler
    
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

#enable_testing()
#add_test(NAME MyTest COMMAND test_config)
