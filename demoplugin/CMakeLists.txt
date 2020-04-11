
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(demoplugin SHARED dp.c)

target_include_directories(
    demoplugin INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)