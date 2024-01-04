#version 430 core

// This should take in an array of transforms
// Transforms should specify:
//  - Which atlas; 
//  - Where in the atlas;
//  - Size; 
//  - Pos;

layout(location = 0) flat out int textureAtlasIdxOut;
layout(location = 1) out vec2 textureCoordsOut;

void main() {
    // The frameBuffer coordinates look like ( 1920, 1080 ); since they represent a pos in a window
    // The OpenGL coordinates look like ( 1, 1 ), since they are normalized.
    // In OpenGL, top left is ( -1, 1 ); in Vulkan, for example, it is ( -1, -1 )

    vec2 vertices[6] = {
        vec2(-0.5, 0.5),    // Top left
        vec2(-0.5, -0.5),   // Bottom left
        vec2(0.5, 0.5),     // Top right
        vec2(0.5, 0.5),     // Top right
        vec2(-0.5, -0.5),   // Bottom left
        vec2(0.5, -0.5),    // Bottom right
    };

    // PLACEHOLDER: The first character
    float top = 6.f;
    float left = 1.f;
    float bottom = 28.f;
    float right = 16.f;

    vec2 textureCoords[6] = {
        vec2(left,  top),
        vec2(left,  bottom),
        vec2(right, top),
        vec2(right, top),
        vec2(left,  bottom),
        vec2(right, bottom),
    };

    gl_Position = vec4(vertices[gl_VertexID], 1, 1);
    textureAtlasIdxOut = 1;
    textureCoordsOut = textureCoords[gl_VertexID];
};
