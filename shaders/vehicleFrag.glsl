#version 330 core

in vec2 TexCoord;
in vec4 specular;
in vec4 diffuse;

layout (std140) uniform Matrices{
    mat4 projectionMatrix;
    mat4 viewingMatrix;
    vec3 eyePos;
};

out vec4 fragColor;
uniform sampler2D sampler;

void main(void){
    vec4 diffuseModifier = diffuse/2 + vec4(0.5,0.5,0.5,1);
    fragColor = texture(sampler, TexCoord)*diffuseModifier + specular;
}
