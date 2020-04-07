cmake_minimum_required(VERSION 3.10)
message("resource path = ${RESOURCE_DIR}")

# set the project name
project(D12Pro)
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DRESOURCE_DIR="${RESOURCE_DIR}")

add_subdirectory(TestApp)
add_subdirectory(Box)
add_subdirectory(Shapes)
add_subdirectory(Common)
add_subdirectory(imgui)
add_subdirectory(ImApp)

