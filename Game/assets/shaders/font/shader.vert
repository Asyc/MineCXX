#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texPos;

struct CharInfo {
    vec2 textureOrigin;
    vec2 textureSize;
};

layout (set = 0, binding = 0) uniform CharInfo asciiCharacterInfo[256];

void main() {
    gl_Position = vec4(pos, 1.0f);
}
