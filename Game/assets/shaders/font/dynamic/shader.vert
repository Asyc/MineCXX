#version 450
#extension GL_EXT_debug_printf : enable

layout (push_constant) uniform PushConstantBlock {
    vec2 pos;
} push_constants;

void main() {
    gl_Position = vec4(push_constants.pos, 0.0f, 1.0f);
}
