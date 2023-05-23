#version 330 core

in vec2 TexCoord;

out vec4 fragColor;

uniform sampler2D sampler;

void main(void){
    fragColor = texture(sampler, TexCoord);
}
