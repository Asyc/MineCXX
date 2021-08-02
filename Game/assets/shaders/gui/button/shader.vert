#version 450 core
#extension GL_EXT_debug_printf : enable

layout (location = 0) in vec2 pos;

layout (location = 0) out vec2 vs_TexPos;

layout (std140, push_constant) uniform PushConstantBlock {
    vec2 texPos[4];
} push_constants;

layout (std140, set = 0, binding = 0) uniform Viewport {
    mat4 projection_matrix;
    float scale_factor;
} viewport;

void main() {
    vs_TexPos = push_constants.texPos[gl_VertexIndex];
    gl_Position = viewport.projection_matrix * vec4(pos.xy, 0.0f, 1.0f);
}
