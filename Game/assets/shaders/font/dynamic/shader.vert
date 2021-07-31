#version 450

layout (push_constant) uniform PushConstantBlock {
    vec2 pos;
} push_constants;

layout (set = 0, binding = 0) uniform Viewport {
    mat4 projection_matrix;
} viewport;

void main() {
    gl_Position = viewport.projection_matrix * vec4(push_constants.pos, 0.0f, 1.0f);
}
