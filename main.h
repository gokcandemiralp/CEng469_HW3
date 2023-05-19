#ifndef MAIN_H
#define MAIN_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace tinyobj;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
          string& data){     ///< [out] The contents of the file
    fstream myfile;

    myfile.open(fileName.c_str(), std::ios::in); // Open the input

    if (myfile.is_open()){
        string curLine;

        while (getline(myfile, curLine)){
            data += curLine;
            if (!myfile.eof()){
                data += "\n";
            }
        }

        myfile.close();
    }
    else{
        return false;
    }
    return true;
}


GLuint createVS(const char* shaderName){
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource)){
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = (GLint)shaderSource.length();
    const GLchar* shader = (const GLchar*)shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = { 0 };
    glGetShaderInfoLog(vs, 1024, &length, output);
    if(strcmp(output,"")){printf("VS compile log: %s\n", output);}

    return vs;
}

GLuint createFS(const char* shaderName){
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource)){
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = (GLint)shaderSource.length();
    const GLchar* shader = (const GLchar*)shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = { 0 };
    glGetShaderInfoLog(fs, 1024, &length, output);
    if(strcmp(output,"")){printf("FS compile log: %s\n", output);}

    return fs;
}

class Mesh;

class Scene{
    public:
    
    float deltaTime,lastFrame;
    float rotationSensivity;
    
    GLuint UBO;
    GLFWwindow* window;
    int gWidth, gHeight;
    float eyeSpeedCoefficient;
    float vehicleAngle;
    glm::vec3 eyePos;
    glm::vec3 eyeFront,eyeUp,eyeSide;
    
    glm::mat4 orientationMatrix;
    glm::mat4 vehicleOrientationMatrix;
    
    int MeshCount;
    vector<Mesh*> Meshs;
    
    Scene();
    Scene(int inputWidth, int inputHeight);
    void renderWithoutVehicle();
    void calculateDirection();
    void calculateFrameTime();
    void movementKeys(GLFWwindow* window);
    void initWindowShape();
    void lookAt();
};

class Mesh{
    public:
    
    Scene *scene;
    int MeshId;
    
    string objDir;
    string texDir;
    string cubeMapDirs[6];
    GLuint VAO, VBO, EBO, FBO, RBO;
    GLuint gProgram, textureID;
    GLuint vertexDataSize, normalDataSize, indexDataSize, texCoordDataSize;
    GLuint vertexEntries, texCoordEntries, faceEntries;
    fastObjMesh* model;
    
    GLfloat* vertexData;
    GLfloat* normalData;
    GLfloat* texCoordData;
    GLuint* indexData;
    
    bool isVehicle;
    float scaleFactor;
    glm::vec3 positionOffset;
    
    Mesh();
    Mesh(Scene *inputScene, string inputObjDir, string inputTexDir);
    Mesh(Scene *inputScene, string inputObjDir, string inputCubeTexDirs[6]);
    bool writeVertexNormal(GLfloat* normalData, int vertexIndex, int normalIndex);
    bool writeVertexTexCoord(GLfloat* texCoordData, int vertexIndex, int texCoordIndex);
    void addFaceElements(int index);
    void initShader(string vertDir, string fragDir);
    void initBuffer(float scaleFactorInput, glm::vec3 positionOffsetInput);
    void initSkyBoxBuffer();
    void render();
    void renderVariation(glm::vec3 positionOffsetInput);
    void renderCubeMap();
};

Scene::Scene(){;}

Scene::Scene(int inputWidth, int inputHeight){
    gWidth = inputWidth;
    gHeight = inputHeight;
    MeshCount = 0;
    deltaTime = 0.0f;
    lastFrame = 0.0f;

    rotationSensivity = 0.5f;
    eyeSpeedCoefficient = 0.0f;
    vehicleOrientationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f,  -12.0f));
    eyePos   = glm::vec3(0.0f, 4.0f,  12.0f);
    eyeFront = glm::vec3(0.0f, 0.0f, -1.0f);
    eyeUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    eyeSide  = glm::vec3(1.0f, 0.0f,  0.0f);
    
    if (!glfwInit()){
        exit(-1);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(gWidth, gHeight, "CENG469_HW2", NULL, NULL);

    if (!window){
        glfwTerminate();
        exit(-1);
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit()){
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }

    char rendererInfo[512] = { 0 };
    strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER));
    strcat(rendererInfo, " - ");
    strcat(rendererInfo, (const char*)glGetString(GL_VERSION));
    glfwSetWindowTitle(window, rendererInfo);
    
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
      
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(glm::mat4));
}

void Scene::renderWithoutVehicle(){
    Meshs[0]->renderCubeMap();
    Meshs[1]->render();
    Meshs[2]->renderVariation(glm::vec3(20.0f,-4.6f,-20.0f));
    Meshs[2]->renderVariation(glm::vec3(-30.0f,-4.6f,-25.0f));
    Meshs[4]->render();
}

void Scene::calculateDirection(){
    // empty for now
}

void Scene::calculateFrameTime(){
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void Scene::movementKeys(GLFWwindow* window){
    int sign = 1;
    glm::quat quatRoll = glm::quat(1.0f,0.0f,0.0f,0.0f);
    glm::quat quatYaw = glm::quat(1.0f,0.0f,0.0f,0.0f);
    glm::quat quatPitch = glm::quat(1.0f,0.0f,0.0f,0.0f);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        eyeSpeedCoefficient = max(-10.0f,eyeSpeedCoefficient - (float)deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        eyeSpeedCoefficient = min(10.0f,eyeSpeedCoefficient + (float)deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        quatRoll = glm::quat(cos(glm::radians(-rotationSensivity)/2),eyeFront*sin(glm::radians(-rotationSensivity)/2));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        quatRoll = glm::quat(cos(glm::radians(rotationSensivity)/2),eyeFront*sin(glm::radians(rotationSensivity)/2));
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        quatYaw = glm::quat(cos(glm::radians(rotationSensivity)/2),eyeUp*sin(glm::radians(rotationSensivity)/2));
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        quatYaw = glm::quat(cos(glm::radians(-rotationSensivity)/2),eyeUp*sin(glm::radians(-rotationSensivity)/2));
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
        quatPitch = glm::quat(cos(glm::radians(rotationSensivity)/2),eyeSide*sin(glm::radians(rotationSensivity)/2));
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        quatPitch = glm::quat(cos(glm::radians(-rotationSensivity)/2),eyeSide*sin(glm::radians(-rotationSensivity)/2));;
    }
    
    //quatRoll = glm::quat(cos(glm::radians(-rotationSensivity)/2),eyeFront*sin(glm::radians(-rotationSensivity)/2));
    //quatYaw = glm::quat(cos(glm::radians(rotationSensivity)/2),eyeUp*sin(glm::radians(rotationSensivity)/2));
    //quatPitch = glm::quat(cos(glm::radians(rotationSensivity)/2),eyeSide*sin(glm::radians(rotationSensivity)/2));
    
    orientationMatrix = glm::toMat4(quatPitch) * glm::toMat4(quatYaw) * glm::toMat4(quatRoll);
    eyeFront = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(eyeFront,1.0f)));
    eyeUp    = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(eyeUp   ,1.0f)));
    eyeSide  = glm::normalize(glm::cross(eyeFront,eyeUp));
    
    (eyeSpeedCoefficient > 0) ? sign = -1 : sign = 1;
    eyePos -= eyeFront * eyeSpeedCoefficient * (0.1f);
    vehicleOrientationMatrix = glm::toMat4(quatPitch) * glm::toMat4(quatYaw) * glm::toMat4(quatRoll) * vehicleOrientationMatrix;
    
    // Bleed off Speeds
    eyeSpeedCoefficient /= 1.01;
}

void Scene::initWindowShape(){
    glViewport(0, 0, gWidth, gHeight);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), gWidth/(float) gHeight, 1.0f, 400.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projectionMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Scene::lookAt(){
    glm::mat4 viewingMatrix = glm::lookAt(eyePos, eyePos + eyeFront, eyeUp);
    
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewingMatrix));
    glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(eyePos));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Mesh::Mesh() {
    ;
}

Mesh::Mesh(Scene *inputScene, string inputObjDir, string inputTexDir) {
    isVehicle = false;
    scene = inputScene;
    objDir = inputObjDir;
    texDir = inputTexDir;
    MeshId = scene->MeshCount;
    scene->Meshs.push_back(this);
    cout << "Mesh Created - MeshId:" << MeshId << "\n";
    scene->MeshCount += 1;
}

Mesh::Mesh(Scene *inputScene, string inputObjDir, string inputCubeTexDirs[6]) {
    isVehicle = false;
    scene = inputScene;
    objDir = inputObjDir;
    for(int i = 0 ; i < 6 ; ++i){
        cubeMapDirs[i] = inputCubeTexDirs[i];
    }
    MeshId = scene->MeshCount;
    scene->Meshs.push_back(this);
    cout << "Mesh Created - MeshId:" << MeshId << "\n";
    scene->MeshCount += 1;
}

bool Mesh::writeVertexNormal(GLfloat* normalData, int vertexIndex, int normalIndex){
    if(normalData[3 * vertexIndex] == 0 &&
       normalData[3 * vertexIndex + 1] == 0 &&
       normalData[3 * vertexIndex + 2] == 0){
        normalData[3 * vertexIndex] = model->normals[3 * normalIndex];
        normalData[3 * vertexIndex + 1] = model->normals[3 * normalIndex + 1];
        normalData[3 * vertexIndex + 2] = model->normals[3 * normalIndex + 2];
        return false;
    }
    else if (normalData[3 * vertexIndex] == model->normals[3 * normalIndex] &&
             normalData[3 * vertexIndex + 1] == model->normals[3 * normalIndex + 1] &&
             normalData[3 * vertexIndex + 2] == model->normals[3 * normalIndex + 2]){
        return false;
    }
    return true;
}

bool Mesh::writeVertexTexCoord(GLfloat* texCoordData, int vertexIndex, int texCoordIndex){
    if(texCoordData[2 * vertexIndex] == 0 && texCoordData[2 * vertexIndex + 1] == 0){
        texCoordData[2 * vertexIndex] = model->texcoords[2 * texCoordIndex];
        texCoordData[2 * vertexIndex + 1] = 1.0f - model->texcoords[2 * texCoordIndex + 1];
        return false;
    }
    else if (texCoordData[2 * vertexIndex] == model->texcoords[2 * texCoordIndex] &&
             texCoordData[2 * vertexIndex + 1] == 1.0f - model->texcoords[2 * texCoordIndex + 1]){
        return false;
    }
    return true;
}

void Mesh::addFaceElements(int index){
    if(writeVertexNormal(normalData, model->indices[index].p,model->indices[index].n) ||
       writeVertexTexCoord(texCoordData, model->indices[index].p,model->indices[index].t)){
        vertexData[vertexEntries] = vertexData[3 * model->indices[index].p];
        vertexData[vertexEntries + 1] = vertexData[3 * model->indices[index].p + 1];
        vertexData[vertexEntries + 2] = vertexData[3 * model->indices[index].p + 2];
        
        normalData[vertexEntries] = model->normals[3 * model->indices[index].n];
        normalData[vertexEntries + 1] = model->normals[3 * model->indices[index].n + 1];
        normalData[vertexEntries + 2] = model->normals[3 * model->indices[index].n + 2];
        
        texCoordData[texCoordEntries] = model->texcoords[2 * model->indices[index].t];
        texCoordData[texCoordEntries + 1] = 1.0f - model->texcoords[2 * model->indices[index].t + 1];
        indexData[index] = vertexEntries/3;
        vertexEntries = vertexEntries + 3;
        texCoordEntries = texCoordEntries + 2;
    }
    else{
        indexData[index] = model->indices[index].p;
    }
}

void Mesh::initShader(string vertDir, string fragDir){
    GLint status, vs, fs;
    
    gProgram = glCreateProgram();
    vs = createVS(vertDir.c_str());
    fs = createFS(fragDir.c_str());
    glAttachShader(gProgram, vs);
    glAttachShader(gProgram, fs);
    glLinkProgram(gProgram);
    glGetProgramiv(gProgram, GL_LINK_STATUS, &status);

    if (status != GL_TRUE){
        cout << "Program link failed for program" << gProgram << endl;
        exit(-1);
    }
}

void Mesh::initBuffer(float scaleFactorInput, glm::vec3 positionOffsetInput){
    scaleFactor = scaleFactorInput;
    positionOffset = positionOffsetInput;
    model = fast_obj_read(objDir.c_str());
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texDir.c_str(), &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    
    stbi_image_free(data);
    
    vertexEntries = model->position_count * 3;
    texCoordEntries = model->position_count * 2;
    faceEntries = model->face_count * 3;
    
    vertexDataSize = vertexEntries * sizeof(GLfloat);
    indexDataSize = faceEntries * sizeof(GLuint);
    
    vertexData  = new GLfloat[faceEntries * 3] ();
    memcpy(vertexData, model->positions, vertexDataSize);
    normalData = new GLfloat[faceEntries * 3] ();
    texCoordData = new GLfloat[faceEntries * 3] ();
    indexData = new GLuint[faceEntries] ();
    
    for (int i = 0; i < model->face_count; ++i){
        for(int j = 0 ; j < 3 ; ++j) {
            addFaceElements(3 * i + j);
        }
    }
    vertexDataSize = vertexEntries * sizeof(GLfloat);
    normalDataSize = vertexEntries * sizeof(GLfloat);
    texCoordDataSize = texCoordEntries * sizeof(GLfloat);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    assert(glGetError() == GL_NONE);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize + texCoordDataSize, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSize, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize, normalDataSize, normalData);
    glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize, texCoordDataSize, texCoordData);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexDataSize));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexDataSize + normalDataSize));

    // done copying; can free now
    delete[] vertexData;
    delete[] normalData;
    delete[] texCoordData;
    delete[] indexData;
    fast_obj_destroy(model);
}

void Mesh::initSkyBoxBuffer(){
    model = fast_obj_read(objDir.c_str());
    int vertexEntries, faceEntries;
    
    vertexEntries = model->position_count * 3;
    faceEntries = model->face_count * 3;
    
    vertexDataSize = vertexEntries * sizeof(GLfloat);
    indexDataSize = faceEntries * sizeof(GLuint);
    GLuint* indexData = new GLuint[faceEntries];
    
    for (int i = 0; i < model->face_count; ++i){
        indexData[3 * i] = model->indices[3 * i].p;
        indexData[3 * i + 1] = model->indices[3 * i + 1].p;
        indexData[3 * i + 2] = model->indices[3 * i + 2].p;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, model->positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // Creates the cubemap texture object
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // This might help with seams on some systems
    
    // Cycles through all the textures and attaches them to the cubemap object
    for (unsigned int i = 0; i < 6; i++){
        int width, height, nrChannels;
        unsigned char* data = stbi_load(cubeMapDirs[i].c_str(), &width, &height, &nrChannels, 0);
        if (data){
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
        else{
            std::cout << "Failed to load texture: " << cubeMapDirs[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // done copying; can free now
    delete[] indexData;
    fast_obj_destroy(model);
}

void Mesh::render(){
    glm::mat4 matS,matT,matR,modelingMatrix;
    glUseProgram(gProgram);
    
    if(isVehicle) {
        glm::mat4 matT1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f,  -12.0f));
        glm::mat4 matT2 = glm::translate(glm::mat4(1.0f), scene->eyePos);
        //matT = glm::translate(glm::mat4(1.0f), positionOffset);
        matS = glm::scale(glm::mat4(1.f), glm::vec3(scaleFactor ,scaleFactor ,scaleFactor));
        modelingMatrix = matT2 * scene->vehicleOrientationMatrix * matS;
    }
    else{
        matR = glm::rotate(glm::mat4(1.0f), glm::radians(scene->vehicleAngle), glm::vec3(0.0f,1.0f,0.0f));
        matS = glm::scale(glm::mat4(1.f), glm::vec3(scaleFactor ,scaleFactor ,scaleFactor));
        matT = glm::translate(glm::mat4(1.0f), positionOffset);
        modelingMatrix = matT * matS * matR;
    }
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(gProgram, "sampler"), 1);
    
    glUniformMatrix4fv(glGetUniformLocation(gProgram, "modelingMatrix"), 1, GL_FALSE, glm::value_ptr(modelingMatrix));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faceEntries , GL_UNSIGNED_INT, 0);
}

void Mesh::renderVariation(glm::vec3 positionOffsetInput){
    positionOffset = positionOffsetInput;
    render();
}

void Mesh::renderCubeMap(){
    glDisable(GL_DEPTH_TEST);
    
    glm::mat4 matS = glm::scale(glm::mat4(1.f), glm::vec3(80.0f ,80.0f ,80.0f));
    glm::mat4 matT = glm::translate(glm::mat4(1.0f), scene->eyePos);
    glm::mat4 matR = glm::rotate(glm::mat4(1.0f), glm::radians(scene->vehicleAngle), glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 modelingMatrix = matT * matR * matS;
    
    glUseProgram(gProgram);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    glUniform1i(glGetUniformLocation(gProgram, "sampler"), 0);
    
    glUniformMatrix4fv(glGetUniformLocation(gProgram, "modelingMatrix"), 1, GL_FALSE, glm::value_ptr(modelingMatrix));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glEnable(GL_DEPTH_TEST);
}

#endif
