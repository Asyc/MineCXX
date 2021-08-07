#version 450 core
#extension GL_EXT_debug_printf : enable

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texPos;

layout (location = 0) out vec2 vs_TexPos;

layout (std140, set = 0, binding = 0) uniform Viewport {
    mat4 projection_matrix;
    float scaleFactor;
} viewport;

layout (push_constant) uniform PushConstantBlock {
    layout (offset = 16) bool viewportAdjust;// Fragment shader consumes 0-16
} push_constants;

void main() {
    vs_TexPos = texPos;

    vec4 position = vec4(pos, 0.0f, 1.0f);
    if (push_constants.viewportAdjust) {
        position.xy *= viewport.scaleFactor;
        position = viewport.projection_matrix * position;
    }

    gl_Position = position;
}
