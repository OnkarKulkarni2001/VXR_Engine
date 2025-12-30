#version 450

layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

// SET 1 — material data
layout(set = 1, binding = 0) uniform sampler2D uAlbedo;
layout(set = 1, binding = 1) uniform sampler2D uNormal;

void main()
{
    outColor = texture(uAlbedo, vUV);
    //outColor = vec4(vUV, 0, 1);
}
