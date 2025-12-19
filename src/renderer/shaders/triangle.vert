#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    vec4 tint;
} pc;

void main() {
    fragColor = inColor;
    gl_Position = ubo.proj * ubo.view * pc.model * vec4(inPos, 0.0, 1.0);
}
