#version 450 core
#extension GL_EXT_debug_printf : enable

layout (location = 0) in vec2 vs_TexPos;

layout (location = 0) out vec4 fs_Color;

layout (set = 0, binding = 3) uniform sampler2D u_TexAtlas;

void main() {
   // debugPrintfEXT("%f, %f", vs_TexPos.x, vs_TexPos.y);
    fs_Color = texture(u_TexAtlas, vs_TexPos);
    if (fs_Color.w <= 0.0f) { discard; }
}