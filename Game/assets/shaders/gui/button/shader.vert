#version 450 core
#extension GL_EXT_debug_printf : enable

layout (location = 0) in vec2 pos;

layout (location = 0) out vec2 vs_TexPos;

layout (std140, push_constant) uniform PushConstantBlock {
    vec2 texPos[4];
} push_constants;

layout (set = 0, binding = 0) uniform Viewport {
    mat4 projection_matrix;
} viewport;

void main() {
    vs_TexPos = push_constants.texPos[gl_VertexIndex];

    // Align black outline

    gl_Position = viewport.projection_matrix * vec4(pos, 0.0f, 1.0f);
}
