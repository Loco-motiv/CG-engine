cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SFML_DIR "C:/libs/SFML/lib/cmake/SFML")
set(SFML_STATIC_LIBRARIES TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(FREETYPE_INCLUDE_DIR_ft2build "C:/libs/freetype/include/freetype2")
set(FREETYPE_INCLUDE_DIR_freetype2 "C:/libs/freetype/include/freetype2/freetype")
set(FREETYPE_LIBRARY "C:/libs/freetype/lib/libfreetype.a")

file(GLOB_RECURSE SOURCES src/*.cpp glad/*.c)

project(CG_engine)

find_package(SFML 3.1 COMPONENTS Window REQUIRED)
find_package(Freetype REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -pedantic -O0 -g -mavx2 -mfma -msse4.2")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include glad/include "C:/libs/stb" "C:/libs/tinybvh")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "C:/libs/tinyobjloader")
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Window opengl32 Freetype::Freetype)