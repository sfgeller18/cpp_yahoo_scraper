cmake_minimum_required(VERSION 3.10)
project(Query)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

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
    "${FORMS_DIR}/*.ui"
    "${INCLUDE_DIR}/*.hpp"
    "${INCLUDE_DIR}/*.h"
)

# Create the executable
add_executable(query ${SOURCES})

# Link against libcurl and Qt::Widgets
target_link_libraries(query PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${CURL_LIBRARIES})
