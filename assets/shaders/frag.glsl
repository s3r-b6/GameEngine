#version 430 core

layout(location=0)flat in uint textureAtlasIdx;
layout(location=1)in vec2 textureCoordsIn;

layout(location=0)out vec4 fragColor;

layout(binding=0)uniform sampler2D fontAtlas;
layout(binding=1)uniform sampler2D textureAtlas1;
layout(binding=2)uniform sampler2D textureAtlas2;
layout(binding=3)uniform sampler2D textureAtlas3;

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
    
    float intensity=max(textureColor.rgb);
    
    vec4 col0=vec4(0.,0.,0.,1.);
    vec4 col1=vec4(.49019607843137253,.1607843137254902,.3254901960784314,1.);
    vec4 col2=vec4(.37254901960784315,.3411764705882353,.3137254901960784,1.);
    vec4 col3=vec4(.5098039215686274,.4588235294117647,.6039215686274509,1.);
    vec4 col4=vec4(1.,.4627450980392157,.6509803921568628,1.);
    vec4 col5=vec4(.7529411764705882,.7568627450980392,.7725490196078432,1.);
    vec4 col6=vec4(.9607843137254902,.9411764705882353,.9058823529411765,1.);
    
    if(intensity<.16)fragColor=col0;
    else if(intensity<.32)fragColor=col1;
    else if(intensity<.48)fragColor=col2;
    else if(intensity<.64)fragColor=col3;
    else if(intensity<.80)fragColor=col4;
    else if(intensity<.9)fragColor=col5;
    else fragColor=col6;
}
