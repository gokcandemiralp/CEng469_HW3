#version 330 core

// All of the following variables could be defined in the OpenGL
// program and passed to this shader as uniform variables. This
// would be necessary if their values could change during runtim.
// However, we will not change them and therefore we define them
// here for simplicity.

vec3 I = vec3(1, 1, 1);          // point light intensity
vec3 ks = vec3(1, 1, 0.95);   // specular reflectance coefficient
vec3 lightPos = vec3(-4, 4, 4);   // light position in world coordinates

uniform mat4 modelingMatrix;
layout (std140) uniform Matrices{
    mat4 projectionMatrix;
    mat4 viewingMatrix;
    vec3 eyePos;
};

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

out vec3 Position;
out vec4 specular;
out vec3 Normal;
out vec2 TexCoord;
out vec4 refEyePosition;

void main(void)
{
    vec4 pWorld = modelingMatrix * vec4(inVertex, 1);
    vec3 nWorld = inverse(transpose(mat3x3(modelingMatrix))) * inNormal;

    vec3 L = normalize(lightPos - vec3(pWorld));
    vec3 V = normalize(eyePos - vec3(pWorld));
    vec3 H = normalize(L + V);
    vec3 N = normalize(nWorld);

    float NdotH = dot(N, H); // for specular component

    vec3 specularColor = I * ks * pow(max(0, NdotH), 100);

    specular = vec4(specularColor, 1);
    Normal = mat3(transpose(inverse(modelingMatrix))) * inNormal;
    refEyePosition = vec4(eyePos,1);
    TexCoord = inTexCoord;

    Position = vec3(modelingMatrix * vec4(inVertex, 1.0));
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1.0);
}

