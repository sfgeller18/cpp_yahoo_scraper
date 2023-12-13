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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MONGOCXX_FLAGS}")

# Find and configure Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Set directories
set(FORMS_DIR "${CMAKE_SOURCE_DIR}/forms")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

# Add include directories
include_directories(${FORMS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})


# Find and link against libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

set_property(SOURCE ${FORMS_DIR}/mainwindow.ui PROPERTY VS_TOOL_OVERRIDE "VCCustomBuildTool")

# Collect source files
file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/UI/*.cpp"
    "${FORMS_DIR}/*.ui"
    "${INCLUDE_DIR}/*.hpp"
    "${INCLUDE_DIR}/*.h"
)

add_executable(query ${SOURCES})
add_dependencies(query time_series)

target_link_libraries(query PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${CURL_LIBRARIES} mongocxx bsoncxx)


