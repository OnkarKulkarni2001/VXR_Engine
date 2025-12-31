#version 450

// ===== Vertex Inputs =====
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

// ===== Scene UBO (set = 0) =====
layout(set = 0, binding = 0) uniform SceneUBO
{
    mat4 view;
    mat4 proj;

    // lighting data exists here but is NOT used in vertex stage
} scene;

// ===== Push Constants (per-object) =====
layout(push_constant) uniform PushConstants
{
    mat4 model;
} pc;

// ===== Outputs to Fragment Shader =====
layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec2 vUV;
layout(location = 2) out vec3 vWorldPos;

void main()
{
    // World position
    vec4 worldPos = pc.model * vec4(inPosition, 1.0);
    vWorldPos = worldPos.xyz;

    // Normal matrix (correct for non-uniform scale)
    mat3 normalMat = mat3(transpose(inverse(pc.model)));
    vNormal = normalize(normalMat * inNormal);

    // UV passthrough
    vUV = inUV;

    // Final clip-space position
    gl_Position = scene.proj * scene.view * worldPos;
}
