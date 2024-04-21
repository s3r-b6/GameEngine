#version 430 core

layout(location=0)flat in uint textureAtlasIdx;
layout(location=1)in vec2 textureCoordsIn;
layout(location=2)in float distanceToCenter;

layout(location=0)out vec4 fragColor;

layout(binding=0)uniform sampler2D fontAtlas;
layout(binding=1)uniform sampler2D textureAtlas1;
layout(binding=2)uniform sampler2D textureAtlas2;
layout(binding=3)uniform sampler2D textureAtlas3;

uniform vec2 screenSize;

float max(vec3 vec){
    if(vec.r>vec.g){
        if(vec.r>vec.b)return vec.r;
        else return vec.b;
    }else{
        if(vec.g>vec.b)return vec.g;
        else return vec.b;
    }
}

void main(){
    vec4 textureColor;
    
    if(textureAtlasIdx==0){
        float red=texelFetch(fontAtlas,ivec2(textureCoordsIn),0).r;
        
        if(red==0.f)discard;
        
        textureColor=red*vec4(1.f,1.f,1.f,1.f);
        fragColor=textureColor;
        return;
    }
    
    if(textureAtlasIdx==1)textureColor=texelFetch(textureAtlas1,ivec2(textureCoordsIn),0);
    else if(textureAtlasIdx==2)textureColor=texelFetch(textureAtlas2,ivec2(textureCoordsIn),0);
    else if(textureAtlasIdx==3)textureColor=texelFetch(textureAtlas3,ivec2(textureCoordsIn),0);
    else discard;
    
    if(textureColor.a==0.f)discard;

    //float maxDistance = length(screenSize) / 2.0;
    //float attenuation = 1.0 - (distanceToCenter / maxDistance);

    //vec4 color = vec4(attenuation, attenuation, attenuation, 1.0) * textureColor;

    fragColor = textureColor;
}
