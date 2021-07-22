#version 450

#define MAX_CHARACTERS 256 / 4

struct AsciiCharNode {
    vec2 size;
};

layout(points) in;
layout(triangle_strip, max_vertices = MAX_CHARACTERS * 4) out;

layout(set = 0, binding = 0) uniform RenderData {
    vec2 pos;
    vec2 scale;

    uint count;
    uint characters[64];
} render_data;

layout (set = 0, binding = 1) uniform AsciiCharTable {
    AsciiCharNode nodes[256];
} ascii_table;

void main() {
    vec2 currentPos = vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y);

    for (uint i = 0; i < render_data.count; i++) {
        uint codepoint = render_data.characters[i];

        float width = ascii_table.nodes[codepoint].size.x * render_data.scale.x;
        float height = ascii_table.nodes[codepoint].size.y * render_data.scale.y;

        gl_Position = vec4(currentPos, 0.0f, 1.0f); // Top-Left
        EmitVertex();

        gl_Position = vec4(currentPos.x, currentPos.y + height, 0.0f, 1.0f);    // Bottom-Left
        EmitVertex();

        gl_Position = vec4(currentPos.x + width, currentPos.y + height, 0.0f, 1.0f); // Bottom-Right
        EmitVertex();

        gl_Position = vec4(currentPos.x + width, currentPos.y, 0.0f, 1.0f); // Top-Right
        EmitVertex();

        EndPrimitive();
    }
}
