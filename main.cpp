#include "main.h"

bool isWireframe = false;
bool cloudToggle = true;

string cubeMapDirs[6] ={
    "textures/right.jpg",
    "textures/left.jpg",
    "textures/top.jpg",
    "textures/bottom.jpg",
    "textures/front.jpg",
    "textures/back.jpg"
};
Scene scene = Scene(800, 600);

Mesh skyBoxMesh(&scene,"objects/cube.obj",cubeMapDirs);
Mesh vehicleMesh = Mesh(&scene,"objects/airplane.obj",
                              "textures/airplane.jpg");
Mesh groundMesh = Mesh(&scene,"objects/ground.obj",
                             "textures/water.jpeg");
Mesh cloudMesh = Mesh(&scene,"objects/cloud.obj",
                             "textures/water.jpeg");

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if(key == GLFW_KEY_L && action == GLFW_PRESS){
        isWireframe = !isWireframe;
        if(isWireframe){
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            cout << "KEY_L PRESSED : Wireframe Mode is ON \n";
        }
        else{
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            cout << "KEY_L PRESSED : Wireframe Mode is OFF \n";
        }
    }
    else if(key == GLFW_KEY_T && action == GLFW_PRESS){
        cloudToggle = !cloudToggle;
        if(cloudToggle){cout << "KEY_T PRESSED : Clouds are Toggled ON \n";}
        else{cout << "KEY_T PRESSED : Clouds are Toggled OFF \n";}
    }
    else if(key == GLFW_KEY_F && action == GLFW_PRESS){
        scene.showFPS = !scene.showFPS;
        if(scene.showFPS){cout << "KEY_F PRESSED : FPS counter Toggled ON \n";}
        else{cout << "KEY_F PRESSED : FPS counter Toggled OFF \n";}
    }
    else if (key == GLFW_KEY_Z){
        if(scene.rayMarchSteps > 1) {--scene.rayMarchSteps;}
        cout << "KEY_Z PRESSED : Ray March Step Count" << scene.rayMarchSteps << "\n";
    }
    else if (key == GLFW_KEY_X){
        if(scene.rayMarchSteps < 200) {++scene.rayMarchSteps;}
        cout << "KEY_X PRESSED : Ray March Step Count" << scene.rayMarchSteps << "\n";
    }
}

void cleanBuffers(){
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void init(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    scene.initWindowShape();
    vehicleMesh.isVehicle = true;
    cloudMesh.isVehicle = true;;
    
    skyBoxMesh.initShader("shaders/skyboxVert.glsl","shaders/skyboxFrag.glsl");
    vehicleMesh.initShader("shaders/vehicleVert.glsl","shaders/vehicleFrag.glsl");
    groundMesh.initShader("shaders/groundVert.glsl","shaders/groundFrag.glsl");
    cloudMesh.initShader("shaders/cloudVert.glsl","shaders/cloudFrag.glsl");
    
    skyBoxMesh.initSkyBoxBuffer();
    vehicleMesh.initBuffer(6.0f, glm::vec3(0.0f,0.0f,0.0f));
    groundMesh.initBuffer(600.0f, glm::vec3(0.0f,0.0f,0.0f));
    cloudMesh.initBuffer(20.0f, glm::vec3(0.0f,0.0f,0.0f));
}

void display(){
    scene.lookAt();

    skyBoxMesh.renderCubeMap();
    vehicleMesh.render();
    groundMesh.render();
    if(cloudToggle){cloudMesh.render();}
}


void mainLoop(GLFWwindow* window){
    while (!glfwWindowShouldClose(window)){
        scene.movementKeys(window);
        scene.calculateFrameTime();
        cleanBuffers();
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main(int argc, char** argv){
    init();
    glfwSetKeyCallback(scene.window, keyboard);
    
    cout << "PRESS ESC TO EXIT \n";
    mainLoop(scene.window); // this does not return unless the window is closed

    glfwDestroyWindow(scene.window);
    glfwTerminate();

    return 0;
}
