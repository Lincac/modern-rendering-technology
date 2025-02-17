cmake_minimum_required(VERSION 3.15)

project(modern-rendering-technology)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(API)

add_subdirectory(HelloWindow)
add_dependencies(HelloWindow API)

add_subdirectory(HelloTriangle)
add_dependencies(HelloTriangle API)

add_subdirectory(Shadow)
add_dependencies(Shadow API)