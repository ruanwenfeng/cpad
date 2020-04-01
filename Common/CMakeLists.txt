
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(common Camera.cpp Camera.h d3dApp.cpp d3dApp.h d3dUtil.cpp 
    d3dUtil.h d3dx12.h DDSTextureLoader.cpp DDSTextureLoader.h GameTimer.cpp GameTimer.h 
    GeometryGenerator.cpp GeometryGenerator.h MathHelper.cpp MathHelper.h UploadBuffer.h)

target_link_libraries(common d3dcompiler.lib D3D12.lib dxgi.lib)
target_include_directories(
    common INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)