#version 330 core

in vec3 TexCoord;

out vec4 fragColor;

uniform samplerCube sampler;

void main(void){
    fragColor = texture(sampler, TexCoord);
}
