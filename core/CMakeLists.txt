
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(core plugin.h core.c core.h)
target_link_libraries(core uv_a apr-2)
target_include_directories(
    core INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/core
)

target_compile_definitions(core 
    PUBLIC -DAPR_DECLARE_STATIC)