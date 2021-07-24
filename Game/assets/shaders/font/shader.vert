#version 450
#extension GL_EXT_debug_printf : enable

layout (location = 0) out vec2 vs_TexPos;

struct VertexData {
    vec2 pos;
    vec2 texPos;
};

struct AsciiNode {
    VertexData vertices[4];
};

struct Character {
    vec2 origin;
    uint codepoint;
};

layout (set = 0, binding = 0) uniform AsciiTable {
    AsciiNode nodes[256];
} ascii_table;

layout (push_constant) uniform PushConstants {
    Character buf[]
} push_constants;

uint extractCodepoint(int charIndex) {
    uint wholeValue = push_constants.buf[charIndex / 2];

    int remainder = charIndex % 2;
    if (remainder == 0) {
        return wholeValue & 0xFFu;
    }

    return (wholeValue & 0xFF00u) / 2;
}

void main() {
    int charIndex = gl_VertexIndex / 4;
    int vertexIndex = gl_VertexIndex % 4;

    uint codepoint = extractCodepoint(charIndex);

    Vertex vertexData = ascii_table.nodes[charIndex].vertices[vertexIndex];
    vec2 vertexPos = ascii_table.nodes[charIndex].vertices[vertexIndex].pos;
    vec2 texPos = ascii_table.nodes[charIndex].vertices[vertexIndex].texPos;

    vs_TexPos = texPos;
    gl_Position = vec4(vertexPos, 0.0f, 1.0f);
}
