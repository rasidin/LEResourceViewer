cmake_minimum_required(VERSION 3.18)

project(LEResourceViewer)
enable_testing()

add_definitions(-DWINDOWS)
add_subdirectory(LimitEngineV2)

link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(LEResourceViewer main.cpp)

target_link_libraries(LEResourceViewer LimitEngine ToolBase LEMath d3d12 dxgi dxguid)

add_test(TestTextureViewer LEResourceViewer ${PROJECT_SOURCE_DIR}/resources/TestTexture.lea)