cmake_minimum_required(VERSION 3.10)
project(Query)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

find_package(mongocxx REQUIRED)

execute_process(
    COMMAND pkg-config --cflags --libs libmongocxx
    OUTPUT_VARIABLE MONGOCXX_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_subdirectory(external/time_series)

include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.14.0)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BUILD_DIR})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MONGOCXX_FLAGS}")

# Find and configure Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Set directories
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/tests")

# Add include directories
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})


# Find and link against libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Collect source files
file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/UI/*.cpp"
    "${INCLUDE_DIR}/*.hpp"
    "${INCLUDE_DIR}/*.h"
)

add_executable(query ${SOURCES})
add_dependencies(query time_series)

include_directories(${CMAKE_SOURCE_DIR}/external/time_series/include)

target_compile_definitions(query PRIVATE -DMKL_ILP64 -DUSE_EIGEN -DUSE_OMP -DUSE_MKL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Wl,--no-as-needed")

# Include directories
target_include_directories(query PRIVATE ${EIGEN3_INCLUDE_DIR} ${MKL_INCLUDE_DIR})

target_link_libraries(query PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${CURL_LIBRARIES} mongocxx bsoncxx ${MKL_CORE_LIBRARY} ${MKL_THREAD_LIBRARY} ${MKL_ILP64_LIBRARY} -lgomp -lpthread -lm -ldl time_series)

# Add tests subdirectory
add_executable(tests)
file(GLOB_RECURSE SOURCES "${TEST_DIR}/*.cpp")

# Link against the tests executable
target_link_libraries(tests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${CURL_LIBRARIES} mongocxx bsoncxx ${MKL_CORE_LIBRARY} ${MKL_THREAD_LIBRARY} ${MKL_ILP64_LIBRARY} -lgomp -lpthread -lm -ldl time_series gtest_main)