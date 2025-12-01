cmake_minimum_required(VERSION 3.16)
project(ImGuiQueueDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output executable into build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release)

# ---------------------------------------------------------
# FetchContent: GLFW (auto-download)
# ---------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        latest
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# ---------------------------------------------------------
# OpenGL
# ---------------------------------------------------------
find_package(OpenGL REQUIRED)

# ---------------------------------------------------------
# ImGui (local folder)
# ---------------------------------------------------------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMGUI_BACKENDS ${IMGUI_DIR}/backends)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_BACKENDS}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp
)

# ---------------------------------------------------------
# Executable
# ---------------------------------------------------------
add_executable(ImGuiQueueDemo
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(ImGuiQueueDemo PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS}
)

# ---------------------------------------------------------
# Linking
# ---------------------------------------------------------
target_link_libraries(ImGuiQueueDemo
    glfw
    OpenGL::GL
)

# ---------------------------------------------------------
# Linux: Add X11 + Xrandr and related libs
# ---------------------------------------------------------
if(UNIX AND NOT APPLE)
    target_link_libraries(ImGuiQueueDemo
        X11
        Xrandr
        Xinerama
        Xcursor
        Xi
    )
endif()

# ---------------------------------------------------------
# macOS frameworks
# ---------------------------------------------------------
if(APPLE)
    target_link_libraries(ImGuiQueueDemo
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
