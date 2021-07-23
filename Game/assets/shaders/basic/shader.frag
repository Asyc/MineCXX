#version 450

layout (location = 0) in vec2 vs_TexPos;

layout(location = 0) out vec4 fs_Color;

layout (set = 0, binding = 0) uniform sampler2D tex_sampler;

void main() {
    fs_Color = texture(tex_sampler, vs_TexPos);
}
