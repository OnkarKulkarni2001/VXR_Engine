cmake_minimum_required(VERSION 3.22)
project(VXR_Engine)

set(CMAKE_CXX_STANDARD 20)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Source files
file(GLOB_RECURSE SRC_FILES 
    "src/*.cpp"
    "src/*.h"
)

add_executable(VXR_Engine ${SRC_FILES})

target_include_directories(VXR_Engine PRIVATE 
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(VXR_Engine PRIVATE 
    Vulkan::Vulkan
)
