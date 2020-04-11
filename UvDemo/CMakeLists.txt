add_executable(UvDemo main.c)

target_link_libraries(UvDemo core)
add_dependencies(UvDemo demoplugin)
