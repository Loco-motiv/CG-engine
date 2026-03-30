cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SFML_DIR "C:/libs/SFML/lib/cmake/SFML")
set(SFML_STATIC_LIBRARIES TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(FREETYPE_INCLUDE_DIR_ft2build "C:/libs/freetype/include")
set(FREETYPE_INCLUDE_DIR_freetype2 "C:/libs/freetype/include/freetype")
set(FREETYPE_LIBRARY "C:/libs/freetype/lib/freetype.a")

file(GLOB_RECURSE SOURCES src/*.cpp glad/*.c)

project(CG_engine)

find_package(SFML 2.6 COMPONENTS graphics REQUIRED)
find_package(Freetype REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic -O0 -g")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include glad/include "C:/libs/stb")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "C:/libs/tinyobjloader")
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-window opengl32 Freetype::Freetype)