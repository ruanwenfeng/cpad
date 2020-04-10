add_executable(UvDemo main.c)

target_link_libraries(UvDemo  uv_a apr-2)
target_compile_definitions(UvDemo 
    PRIVATE -DAPR_DECLARE_STATIC)