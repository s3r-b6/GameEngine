#version 430 core

struct Transform {
    ivec2 atlasOffset;
    ivec2 spriteSize;

    vec2 pos;
    vec2 size;

    uint atlasIdx;
    uint padding[3];
};

layout(std430, binding = 0) buffer TransformSBO {
    Transform transforms[];
};

uniform vec2 screenSize;

layout(location = 0) flat out uint textureAtlasIdxOut;
layout(location = 1) out vec2 textureCoordsOut;

void main() {
    Transform t = transforms[gl_InstanceID];

    // In OpenGL, top left is ( -1, 1 );
    vec2 vertices[6] = {
        t.pos,
        t.pos + vec2(0.f, t.size.y),
        t.pos + vec2(t.size.x, 0.f),
        t.pos + vec2(t.size.x, 0.f),
        t.pos + vec2(0.f, t.size.y),
        t.pos + t.size,
        };

    float top = t.atlasOffset.y;
    float left = t.atlasOffset.x;
    float bottom = t.atlasOffset.y + t.spriteSize.y;
    float right = t.atlasOffset.x + t.spriteSize.x;

    vec2 textureCoords[6] = {
        vec2(left, top),
        vec2(left, bottom),
        vec2(right, top),
        vec2(right, top),
        vec2(left, bottom),
        vec2(right, bottom),
        };

    { // Invert the Y (openGL) and normalize
        vec2 vertexPos = vertices[gl_VertexID];
        vertexPos.y = -vertexPos.y + screenSize.y;
        vertexPos = 2.f * (vertexPos / screenSize) - 1.f;
        gl_Position = vec4(vertexPos, 0.f, 1.f);
    }

    textureAtlasIdxOut = t.atlasIdx;
    textureCoordsOut = textureCoords[gl_VertexID];
};
