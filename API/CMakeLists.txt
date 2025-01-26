cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build types." FORCE)
endif()

project(API)

include_directories(${PROJECT_SOURCE_DIR}/GLAD/inc)
include_directories(${PROJECT_SOURCE_DIR}/GLFW/inc)
include_directories(${PROJECT_SOURCE_DIR}/GLM/inc)
include_directories(${PROJECT_SOURCE_DIR}/TOOL/inc)


file(GLOB_RECURSE SRC_FILES 
    "${PROJECT_SOURCE_DIR}/*.c"
    "${PROJECT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE HEADER_FILES 
    "${PROJECT_SOURCE_DIR}/*.h" 
    "${PROJECT_SOURCE_DIR}/*.hpp"
    )

function(create_source_groups)
    foreach(FILE IN ITEMS ${ARGN})
        get_filename_component(ABS_PATH "${FILE}" ABSOLUTE)
        file(RELATIVE_PATH REL_PATH "${PROJECT_SOURCE_DIR}" "${ABS_PATH}")
        get_filename_component(PATH "${REL_PATH}" PATH)
        string(REGEX REPLACE "^${PROJECT_NAME}/" "" GROUP_PATH "${PATH}")
        string(REPLACE "/" "\\" GROUP_PATH "${GROUP_PATH}")
        source_group("${GROUP_PATH}" FILES "${FILE}")
    endforeach()
endfunction()

create_source_groups(${SRC_FILES})
create_source_groups(${HEADER_FILES})

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${HEADER_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE _GLFW_WIN32)