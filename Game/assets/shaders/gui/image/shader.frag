#version 450 core

layout (location = 0) in vec2 texPos;
layout (location = 0) out vec4 fs_Color;

layout (set = 0, binding = 0) uniform sampler2D imageSampler;

void main() {
    float x = 34.0f / 62.0f;
    fs_Color = texture(imageSampler, texPos);
    if (fs_Color.w <= 0.0f) discard;
}
