#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

// Set=0, Binding=0 (we'll create this in C++)
layout(set = 0, binding = 0) uniform UBO
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main()
{
    fragColor = inColor;
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 1.0);
}
