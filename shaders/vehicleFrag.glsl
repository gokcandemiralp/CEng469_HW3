#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec4 specular;
in vec3 Position;
in vec4 refEyePosition;

layout (std140) uniform Matrices{
    mat4 projectionMatrix;
    mat4 viewingMatrix;
    vec3 eyePos;
};

out vec4 fragColor;

uniform sampler2D sampler;
uniform samplerCube skybox;

void main(void){
    vec3 I = normalize(Position-vec3(refEyePosition));
    vec3 R = reflect(I, normalize(Normal));
    fragColor = texture(sampler, TexCoord) + specular;
    // fragColor = vec4(texture(skybox, Position));
    // fragColor = texture(sampler, TexCoord) + specular;
    // fragColor = vec4(normal,1);
}
