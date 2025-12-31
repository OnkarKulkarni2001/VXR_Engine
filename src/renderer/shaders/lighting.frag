#version 450

#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS  16

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec3 vWorldPos;

layout(location = 0) out vec4 outColor;

struct DirectionalLight
{
    vec3 direction;
    float intensity;
    vec3 color;
    float padding;
};

struct PointLight
{
    vec3 position;
    float radius;
    vec3 color;
    float intensity;
};

struct SpotLight
{
    vec3 position;
    float range;
    vec3 direction;
    float innerCutoff;
    vec3 color;
    float outerCutoff;
};

layout(set = 0, binding = 0) uniform SceneUBO
{
    mat4 view;
    mat4 proj;

    DirectionalLight sun;

    uint pointLightCount;
    uint spotLightCount;
    vec2 padding;

    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight  spotLights[MAX_SPOT_LIGHTS];
} scene;

layout(set = 1, binding = 0) uniform sampler2D albedoTex;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 albedo = texture(albedoTex, vUV).rgb;

    vec3 result = vec3(0.0);

    // Directional light
    {
        vec3 L = normalize(-scene.sun.direction);
        float diff = max(dot(N, L), 0.0);
        result += diff * scene.sun.color * scene.sun.intensity;
    }

    // Point lights
    for (uint i = 0; i < scene.pointLightCount; i++)
    {
        PointLight l = scene.pointLights[i];
        vec3 L = l.position - vWorldPos;
        float dist = length(L);

        if (dist < l.radius)
        {
            L = normalize(L);
            float diff = max(dot(N, L), 0.0);
            float atten = 1.0 - (dist / l.radius);
            result += diff * l.color * l.intensity * atten;
        }
    }
    vec3 ambient = albedo * 0.15;
    outColor = vec4(ambient + albedo * result, 1.0);
}
