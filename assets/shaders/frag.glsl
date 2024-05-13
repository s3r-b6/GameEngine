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

float random(vec2 st){
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {
    vec4 textureColor;
    switch (textureAtlasIdx) {
        case 0: {
            float red = texelFetch(fontAtlas, ivec2(textureCoordsIn), 0).r;
            if(red == 0.f) discard;
            textureColor = vec4(red);
        } break;
        case 1: textureColor = texelFetch(textureAtlas1, ivec2(textureCoordsIn), 0); break;
        case 2: textureColor = texelFetch(textureAtlas2, ivec2(textureCoordsIn), 0); break;
        case 3: textureColor = texelFetch(textureAtlas3, ivec2(textureCoordsIn), 0); break;
        default: discard;
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

    vec2 uv = gl_FragCoord.xy / screenSize.xy;
    float noise = random(uv) * 2.0 - 1.0;
    fragColor.rgb += noise * 0.015;

    fragColor.g *= (sin(gl_FragCoord.y * 0.8) + 1.0) * 0.15 + 1.0;
    fragColor.rb *= (cos(gl_FragCoord.y  * 0.8) + 1.0) * 0.335 + 1.0;
}
