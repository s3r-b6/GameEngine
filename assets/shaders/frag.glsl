#version 430 core

layout(location=0) flat in uint textureAtlasIdx;
layout(location=1) in vec2 textureCoordsIn;
layout(location=2) in float distanceToCenter;

layout(location=0) out vec4 fragColor;

layout(binding=0) uniform sampler2D fontAtlas;
layout(binding=1) uniform sampler2D textureAtlas1;
layout(binding=2) uniform sampler2D textureAtlas2;
layout(binding=3) uniform sampler2D textureAtlas3;

uniform vec2 screenSize;

void saturate(inout vec4 vec) {
    float avg = (vec.r + vec.g + vec.b) / 3.0;
    vec.rgb += (vec.rgb - avg) * 0.11;
    vec = clamp(vec, 0.0, 1.0);
}

void main() {
    vec4 textureColor;
    
    if(textureAtlasIdx == 0){
        float red = texelFetch(fontAtlas, ivec2(textureCoordsIn), 0).r;
        
        if(red == 0.0) discard;
        
        textureColor = vec4(red);
        fragColor = textureColor;
        return;
    }
    
    if (textureAtlasIdx == 1) textureColor = texelFetch(textureAtlas1, ivec2(textureCoordsIn), 0);
    else if (textureAtlasIdx == 2) textureColor = texelFetch(textureAtlas2, ivec2(textureCoordsIn), 0);
    else if (textureAtlasIdx == 3) textureColor = texelFetch(textureAtlas3, ivec2(textureCoordsIn), 0);
    else discard;

    if (textureColor.a == 0.0) discard;

    fragColor = textureColor;
    fragColor.g *= (sin(gl_FragCoord.y * 0.7) + 1.0) * 0.15 + 1.0;
    fragColor.rb *= (cos(gl_FragCoord.y  * 0.7) + 1.0) * 0.135 + 1.0;
    saturate(fragColor);
}

