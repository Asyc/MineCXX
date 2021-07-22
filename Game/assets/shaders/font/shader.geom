#version 450

struct AsciiCharNode {
    vec2 size;
};

layout(points) in;
layout(triangle_strip, max_vertices = 256) out;

layout(set = 0, binding = 0) uniform RenderData {
    vec2 pos;
    vec2 scale;

    int count;
    int characters[64];
} render_data;

layout (set = 0, binding = 1) uniform AsciiCharTable {
    AsciiCharNode nodes[256];
} ascii_table;

layout (location = 0) out vec2 gs_TexCoord;

vec4 createPosition(vec2 pos) {
    return vec4(pos.x, pos.y, 0.0f, 1.0f);
}

vec4 createPosition(float x, float y) {
    return vec4(x, y, 0.0f, 1.0f);
}

void main() {
    vec2 currentPos = vec2(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y);

    for (int i = 0; i < 64; i++) {
        if (i >= render_data.count) break;

        int codepoint = render_data.characters[i];

        float width = ascii_table.nodes[codepoint].size.x * render_data.scale.x;
        float height = ascii_table.nodes[codepoint].size.y * render_data.scale.y;

        gl_Position = createPosition(currentPos);   // Top-Left
        gs_TexCoord = vec2(0.0f, 1.0f);
        EmitVertex();

        gl_Position = createPosition(currentPos.x, currentPos.y + height);    // Bottom-Left
        gs_TexCoord = vec2(0.0f, 0.0f);
        EmitVertex();

        gl_Position = createPosition(currentPos.x + width, currentPos.y + height);  // Bottom-Right
        gs_TexCoord = vec2(1.0f, 0.0f);
        EmitVertex();

        gl_Position = createPosition(currentPos.x + width, currentPos.y);   // Top-Right
        gs_TexCoord = vec2(1.0f, 1.0f);
        EmitVertex();

        EndPrimitive();
    }
}
