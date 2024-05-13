#version 430 core

layout(location=0) flat in uint textureAtlasIdx;
layout(location=1) in vec2 textureCoordsIn;
layout(location=2) in float distanceToCenter;

layout(location=0) out vec4 fragColor;

layout(binding=0) uniform sampler2D fontAtlas;
layout(binding=1) uniform sampler2D textureAtlas1;
layout(binding=2) uniform sampler2D textureAtlas2;
layout(binding=3) uniform sampler2D textureAtlas3;
layout(binding=4) uniform sampler2D colorPalette;

uniform vec2 screenSize;
uniform bool palletize;

void main() {
    vec4 textureColor;
    
    if(textureAtlasIdx == 0) {
        float red = texelFetch(fontAtlas, ivec2(textureCoordsIn), 0).r;
        if(red == 0.f) discard;
        textureColor = red * vec4(1.f, 1.f, 1.f, 1.f);
        fragColor = textureColor;
    } else if (textureAtlasIdx == 1) {
        textureColor = texelFetch(textureAtlas1, ivec2(textureCoordsIn), 0);
    } else if (textureAtlasIdx == 2) {
        textureColor = texelFetch(textureAtlas2, ivec2(textureCoordsIn), 0);
    } else if (textureAtlasIdx == 3) {
        textureColor = texelFetch(textureAtlas3, ivec2(textureCoordsIn), 0);
    } else {
        discard;
    }

    if (textureColor.a == 0.0) discard;


    if (palletize) {
        float minDistance = 9999.0;
        int selectedIndex = 0;
        for (int i = 0; i < 64; i++) {
            vec4 paletteColor = texelFetch(colorPalette, ivec2(i % 8, i / 8), 0).rgba;
            float distance = distance(textureColor.rgb, paletteColor.rgb);
            if (distance < minDistance) {
                minDistance = distance;
                selectedIndex = i;
            }
        }
        fragColor = texelFetch(colorPalette, ivec2(selectedIndex % 8, selectedIndex / 8), 0).rgba;
    } else {
        fragColor = textureColor;
    }

    fragColor.g *= (sin(gl_FragCoord.y * 0.8) + 1.0) * 0.15 + 1.0;
    fragColor.rb *= (cos(gl_FragCoord.y  * 0.8) + 1.0) * 0.335 + 1.0;
}
