#version 450

layout (location = 0) in vec3 pos;

layout (location = 0) out vec4 vs_FragColor;

layout (binding = 0) uniform Options {
vec4 color;
} render_options;

void main() {
    vs_FragColor = render_options.color;
    gl_Position = vec4(pos, 1.0f);
}
