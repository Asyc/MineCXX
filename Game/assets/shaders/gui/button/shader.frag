#version 450 core

layout (location = 0) in vec2 texPos;
layout (location = 0) out vec4 fs_Color;

layout (set = 0, binding = 1) uniform sampler2D imageSampler;

void main() {
    vec4 texture = texture(imageSampler, texPos);
    if (texture.w <= 0.0f) discard;

    fs_Color = texture;
}
