#version 430 core

layout(location = 0) flat in uint textureAtlasIdx;
layout(location = 1) in vec2 textureCoordsIn;

layout(location = 0) out vec4 fragColor;

// TODO: Maybe this does not allow for more complex scenarios
// research uniform sampler2DArray textureArray;
// or maybe a vec4 textureData[];

// May be this is too naive, but for now allow for 4 different textures
layout(binding = 0) uniform sampler2D textureAtlas0;
layout(binding = 1) uniform sampler2D textureAtlas1;
layout(binding = 2) uniform sampler2D textureAtlas2;
layout(binding = 3) uniform sampler2D textureAtlas3;

void main() {
    vec4 textureColor;

    if (textureAtlasIdx == 0) textureColor = texelFetch(textureAtlas0, ivec2(textureCoordsIn), 0);
    else if (textureAtlasIdx == 1) textureColor = texelFetch(textureAtlas1, ivec2(textureCoordsIn), 0);
    else if (textureAtlasIdx == 2) textureColor = texelFetch(textureAtlas2, ivec2(textureCoordsIn), 0);
    else if (textureAtlasIdx == 3) textureColor = texelFetch(textureAtlas3, ivec2(textureCoordsIn), 0);
    else discard;

    if (textureColor.a == 0.f) discard;

    fragColor = textureColor;
}
