add_executable(UvDemo main.c)

target_link_libraries(UvDemo core)
add_dependencies(UvDemo demoplugin)
target_compile_definitions(UvDemo 
    PRIVATE -DAPR_DECLARE_STATIC)