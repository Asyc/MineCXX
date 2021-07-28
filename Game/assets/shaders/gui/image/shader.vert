#version 450 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texPos;

layout (location = 0) out vec2 vs_TexPos;

void main() {
    vs_TexPos = texPos;
    gl_Position = vec4(pos, 0.0f, 1.0f);
}
