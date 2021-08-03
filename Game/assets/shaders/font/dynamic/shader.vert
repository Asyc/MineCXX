#version 450

layout (push_constant) uniform PushConstantBlock {
    vec2 pos;
} push_constants;

layout (set = 0, binding = 0) uniform Viewport {
    mat4 projectionMatrix;
    float scaleFactor;
} viewport;

void main() {
    vec3 scaledPosition = vec3(viewport.scaleFactor) * vec3(push_constants.pos, 0.0f);
    gl_Position = viewport.projectionMatrix * vec4(scaledPosition, 1.0f);
}
