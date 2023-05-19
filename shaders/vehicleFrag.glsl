#version 330 core

in vec2 TexCoord;
in vec4 specular;

layout (std140) uniform Matrices{
    mat4 projectionMatrix;
    mat4 viewingMatrix;
    vec3 eyePos;
};

out vec4 fragColor;
uniform sampler2D sampler;

void main(void){
    fragColor = texture(sampler, TexCoord) + specular;
}
