cmake_minimum_required(VERSION 3.15)
project(VXR_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------
# Engine source files
# --------------------------------
file(GLOB_RECURSE ENGINE_SOURCES
    src/*.cpp
    src/*.h
)

# --------------------------------
# Vulkan SDK
# --------------------------------
find_package(Vulkan REQUIRED)

# --------------------------------
# GLFW (from vcpkg)
# --------------------------------
find_package(glfw3 CONFIG REQUIRED)

# --------------------------------
# GLM (math library)
# --------------------------------
find_package(glm CONFIG REQUIRED)


# --------------------------------
# Engine executable
# --------------------------------
add_executable(VXR_Engine ${ENGINE_SOURCES}  "src/renderer/VulkanVertexBuffer.h" "src/renderer/VulkanVertexBuffer.cpp"  "src/renderer/Vertex.h" "src/renderer/TriangleData.h" "src/renderer/UniformBufferObject.h" "src/renderer/VulkanUniformBuffers.h" "src/renderer/VulkanUniformBuffers.cpp" "src/renderer/VulkanDescriptors.h" "src/renderer/VulkanDescriptors.cpp" "src/renderer/CameraUBO.h" "src/renderer/VulkanCameraBuffers.h" "src/renderer/VulkanIndexBuffer.h" "src/renderer/VulkanIndexBuffer.cpp"  "src/renderer/Mesh.h" "src/renderer/Mesh.cpp" "src/renderer/Transform.h"  "src/renderer/Model.h" "src/renderer/Model.cpp" "src/renderer/PushConstants.h" "src/renderer/RenderObject.h")

# --------------------------------
# Link libraries
# --------------------------------
target_link_libraries(VXR_Engine
    PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)

# --------------------------------
# Include paths
# --------------------------------
target_include_directories(VXR_Engine
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

# --------------------------------
# Shader compilation (GLSL → SPIR-V)
# --------------------------------

# Find glslc
find_program(GLSLC_EXECUTABLE
    NAMES glslc
    HINTS
        "$ENV{VULKAN_SDK}/Bin"
        "$ENV{VULKAN_SDK}/Bin32"
)

if (NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found. Make sure Vulkan SDK is installed and VULKAN_SDK is set.")
endif()

set(SHADER_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/renderer/shaders)
set(SHADER_BINARY_DIR ${CMAKE_BINARY_DIR}/shaders)

file(GLOB SHADER_SOURCES
    ${SHADER_SOURCE_DIR}/*.vert
    ${SHADER_SOURCE_DIR}/*.frag
)

set(SPIRV_SHADERS)

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV_FILE ${SHADER_BINARY_DIR}/${FILE_NAME}.spv)

    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        COMMAND ${GLSLC_EXECUTABLE}
                --target-env=vulkan1.3
                ${SHADER}
                -o ${SPIRV_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${FILE_NAME}"
    )

    list(APPEND SPIRV_SHADERS ${SPIRV_FILE})
endforeach()

add_custom_target(Shaders ALL
    DEPENDS ${SPIRV_SHADERS}
)

add_dependencies(VXR_Engine Shaders)

# --------------------------------
# Copy shaders next to executable
# --------------------------------
add_custom_command(
    TARGET VXR_Engine
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:VXR_Engine>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SHADER_BINARY_DIR}
        $<TARGET_FILE_DIR:VXR_Engine>/shaders
    COMMENT "Copying compiled shaders to runtime directory"
)
