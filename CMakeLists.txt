cmake_minimum_required(VERSION 3.15)
project(VXR_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather engine source files
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/*.h
)

# Add GLFW build from source
add_subdirectory(extern/glfw)
include_directories(extern/glfw/include)

# Find Vulkan (installed with Vulkan SDK)
find_package(Vulkan REQUIRED)

# Build engine executable
add_executable(VXR_Engine ${ENGINE_SOURCES} "src/renderer/VulkanInstance.h" "src/renderer/VulkanInstance.cpp" "src/renderer/VulkanDevice.h" "src/renderer/VulkanDevice.cpp")

# Link engine with libraries
target_link_libraries(VXR_Engine
    PRIVATE
        Vulkan::Vulkan
        glfw
)

# Include engine headers
target_include_directories(VXR_Engine PRIVATE ${PROJECT_SOURCE_DIR}/src)
