#version 450 core

layout (location = 0) in vec2 vs_TexPos;

layout (location = 0) out vec4 fs_Color;

layout (set = 0, binding = 3) uniform sampler2D u_TexAtlas;

void main() {
    fs_Color = texture(u_TexAtlas, vs_TexPos);
    if (fs_Color.w <= 0.0f) { discard; }
}