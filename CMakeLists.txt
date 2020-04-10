cmake_minimum_required(VERSION 3.10)
message("resource path = ${RESOURCE_DIR}")

# set the project name
project(D12Pro)
include_directories(
    # ".\\libuv\\include"
    "apr/include"
    "build/apr"
)

IF (CMAKE_BUILD_TYPE MATCHES "Debug" 
        OR CMAKE_BUILD_TYPE MATCHES "None")
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is Debug")
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Release")
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is Release")
ELSEIF (CMAKE_BUILD_TYPE MATCHES "RelWitchDebInfo")
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is RelWitchDebInfo")
ELSEIF (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    MESSAGE(STATUS "CMAKE_BUILD_TYPE is MinSizeRel")
ELSE ()
    MESSAGE(STATUS "unknown CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
ENDIF ()


add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DRESOURCE_DIR="${RESOURCE_DIR}")

add_subdirectory(TestApp)
add_subdirectory(Box)
add_subdirectory(Shapes)
add_subdirectory(Common)
add_subdirectory(imgui)
add_subdirectory(ImApp)

add_subdirectory(libuv)
add_subdirectory(UvDemo)
add_subdirectory(apr)


add_subdirectory(core)
