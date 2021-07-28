#version 450 core

layout (location = 0) in vec2 texPos;
layout (location = 0) out vec4 fs_Color;

layout (set = 0, binding = 0) uniform sampler2D imageSampler;

void main() {
    fs_Color = texture(imageSampler, texPos);
}
