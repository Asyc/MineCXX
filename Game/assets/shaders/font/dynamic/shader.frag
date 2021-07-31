#version 450 core

layout (location = 0) in vec2 gs_TexPos;

layout (location = 0) out vec4 fs_FragColor;

layout (set = 0, binding = 2) uniform sampler2D ascii_sampler;

layout (std140, set = 1, binding = 0) uniform Options {
    vec4 color;
    float scale;
    bool center;
} string_options;

void main() {
    fs_FragColor = texture(ascii_sampler, gs_TexPos) * string_options.color;
    if (fs_FragColor.w <= 0.0f) discard;
}