cmake_minimum_required(VERSION 3.14)
project(OrderBook)

# 1. Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Find External Dependencies
# GLFW is a system dependency and must be installed on your machine.
find_package(glfw3 REQUIRED) 
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# 3. Define Source Files
set(SOURCES
    main.cpp
    
    # Glad (OpenGL Loader)
    lib/glad/src/glad.c
    
    # ImGui Core
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    
    # ImGui Backends
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    
    # ImPlot
    lib/implot/implot.cpp
    lib/implot/implot_items.cpp
)

# 4. Create the Executable
add_executable(OrderBook ${SOURCES})

# 5. Setup Include Directories
target_include_directories(OrderBook PRIVATE
    .                       # For your headers (order.h, exchange.h, etc.)
    lib                     # For <imgui/imgui.h>, <implot/implot.h>
    lib/glad/include        # For <glad/glad.h>
)

# 6. Link Libraries
target_link_libraries(OrderBook PRIVATE
    glfw
    OpenGL::GL
    Threads::Threads
    ${CMAKE_DL_LIBS}        # Needed for dynamic linking (dlopen) on Linux
)