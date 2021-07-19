#version 450

layout(location = 0) out vec4 fs_color;

layout (binding = 0) uniform ColorUniform {
vec4 color;
} color_ubo;

layout (binding = 1) uniform sampler2D tex_sampler;

void main() {
    fs_color = color_ubo.color;
}
