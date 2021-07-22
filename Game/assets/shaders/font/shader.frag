#version 450

layout (location = 0) in vec2 gs_TexCoord;

layout (location = 0) out vec4 fs_FragColor;

void main() {
    fs_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
