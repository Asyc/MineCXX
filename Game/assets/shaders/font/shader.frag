#version 450

layout (location = 0) in vec2 gs_TexPos;
layout (location = 0) out vec4 fs_FragColor;

layout (set = 0, binding = 1) uniform sampler2D ascii_sampler;

void main() {
    fs_FragColor = texture(ascii_sampler, gs_TexPos);
}