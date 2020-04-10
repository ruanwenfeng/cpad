
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(core plugin.h core.c)

target_include_directories(
    core INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)