#version 330 core

uniform mat4 modelingMatrix;

layout(location=0) in vec3 aPos;
layout (std140) uniform Matrices{
    mat4 projectionMatrix;
    mat4 viewingMatrix;
    vec3 eyePos;
};

out vec4 modelingCoordinate;

void main(void){
    modelingCoordinate = modelingMatrix * vec4(aPos, 1);
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(aPos, 1);
}
