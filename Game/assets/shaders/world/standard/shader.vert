#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texPos;

layout (location = 0) out vec2 vs_TexPos;

layout (push_constant) uniform PushConstantBlock {
    vec2 chunkPos;
} push_constants;

layout (set = 0, binding = 0) uniform Viewport {
    mat4 projectionMatrix;
} viewport;

struct ChunkBlockData {
    vec3 position;
    int blockId;
};

struct BlockData {
    vec4 atlasPosition;
};

layout (set = 0, binding = 1) uniform BlockIdMap {
    BlockData idMap[256];
} blockRegistry;

layout (set = 0, binding = 2) readonly buffer SSBO {
    ChunkBlockData blocks[16 * 256 * 16];
} worldData;

void main() {
    ChunkBlockData blockRenderData = worldData.blocks[gl_InstanceIndex];

    vec4 atlasPosition = blockRegistry.idMap[blockRenderData.blockId].atlasPosition;
    vec2 interpolatedTextureUV = atlasPosition.xy + (atlasPosition.zw * texPos);
    vec3 translatedPosition = blockRenderData.position + pos;

    vs_TexPos = interpolatedTextureUV;
    gl_Position = vec4(translatedPosition, 1.0f);
}