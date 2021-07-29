#version 450 core
#extension GL_EXT_debug_printf : enable

layout (location = 0) in vec2 pos;

layout (location = 0) out vec2 vs_TexPos;

layout (std140, push_constant) uniform PushConstantBlock {
    vec2 texPos[4];
} push_constants;

void main() {
    vs_TexPos = push_constants.texPos[gl_VertexIndex];
    gl_Position = vec4(pos, 0.0f, 1.0f);
}
