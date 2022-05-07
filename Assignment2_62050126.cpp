#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

#include <ctime>
#include <chrono>
#include <thread>

#include "Shader.h"
#include "Window.h"
#include "Mesh.h"

using std::chrono::duration_cast;   
using std::chrono::milliseconds;    
using std::chrono::seconds;         
using std::chrono::system_clock;    

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

bool direction = true;
bool camMoving = false;
float triOffset = 2.5f;
float triMaxOffset = 0.0f;
float CherryOffset = 3.4f;
float honeyOffset = 0.35f;
float honeyMaxOffset = 0.5f;
float sauceIncrement = 0.05; //เคลื่อนที่ด้วยความเร็ว 0.05 pixel per sec
float triIncrement = 0.5;

//Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateObject()
{
    GLfloat rectVertices[] =
    {
        -1.0f, -1.0f, 0.0f,     //0
        -1.0f, 1.0f, 0.0f,      //1
        1.0f, 1.0f, 0.0f,       //2
        1.0f, -1.0f, 0.0f,      //3
    };

    unsigned int rectIndices[] = 
    {
        0, 2, 1,
        0, 3, 2,
    };

    Mesh *rect = new Mesh();
    rect->CreateMesh(rectVertices, rectIndices, 12, 6);
    meshList.push_back(rect);

    GLfloat triangleVertices[] =
    {
        -1.0f, -1.0f, 0.0f,     //0
        1.0f, -1.0f, 0.0f,      //1
        0.0f, 1.0f, 0.0f,       //2
    };

    unsigned int triangleIndices[] = 
    {
        0, 1, 2,
    };

    Mesh *triangle = new Mesh();
    triangle->CreateMesh(triangleVertices, triangleIndices, 9, 3);
    meshList.push_back(triangle);

    GLfloat circleVertices[] =
    {
        0.0f, 0.0f, 0.0f,       //0
        1.0f, 0.0f, 0.0f,       //1
        0.94f, 0.35f, 0.0f,     //2
        0.71f, 0.71f, 0.0f,     //3
        0.35f, 0.94f, 0.0f,     //4
        0.0f, 1.0f, 0.0f,       //5
    };

    unsigned int circleIndices[] = 
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
    };

    Mesh *circle = new Mesh();
    circle->CreateMesh(circleVertices, circleIndices, 18, 12);
    meshList.push_back(circle);

}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

void Update(long elapsedTime){

    if(camMoving)
    {
        if (triOffset >= triMaxOffset) triOffset -= (triIncrement * elapsedTime / 1000.0);
        if (CherryOffset >= triMaxOffset) CherryOffset -= (triIncrement * elapsedTime / 1000.0); 
        if (honeyOffset <= honeyMaxOffset) honeyOffset += (sauceIncrement * elapsedTime / 1000.0); 
    }
}

void KeyboardHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        //pree S button to start project
        camMoving = true;
    }
    
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT);
    mainWindow.initialise();

    CreateObject();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformColor = 0;
    glm::vec4 color;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

    //starting animation time
    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    //current animation time
    long currentTime = startTime;
    //last frame animation time
    long lastTime = currentTime;
    //elapsed time
    long elapsedTime;

    glfwSetKeyCallback(mainWindow.getWindow(), KeyboardHandler);
    //Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        elapsedTime = currentTime - lastTime;
        
        lastTime = currentTime; //update time

        //Get + Handle user input events
        glfwPollEvents();

        Update(elapsedTime);

        //Clear window
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw here
        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformColor = shaderList[0].GetColorLocation();
        glm::mat4 model (1.0f);

        //wink1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, triOffset + 0.4f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 1.0f));
        color = glm::vec4(211.0/255.0, 209.0/255.0, 212.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //wink2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.22f, 0.12f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 1.0f));
        color = glm::vec4(211.0/255.0, 209.0/255.0, 212.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        
        //wink3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.41f, 0.01f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.008f, 0.008f, 1.0f));
        color = glm::vec4(211.0/255.0, 209.0/255.0, 212.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        
        //wink4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.39f, -0.04f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 1.0f));
        color = glm::vec4(211.0/255.0, 209.0/255.0, 212.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //Blueberry1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.03f, triOffset-0.25f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //Blueberry2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.07f, triOffset-0.14f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.035f, 0.035f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //Blueberry3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.35f, triOffset-0.03f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.035f, 0.035f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //Blueberry4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.32f, -0.13f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.025f, 0.025f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //Blueberry5
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.3f, triOffset + 0.28f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.025f, 0.025f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        
        //Blueberry6
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, triOffset + 0.38f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.035f, 0.035f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //Blueberry7
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.21f, 0.1f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.035f, 0.035f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //Blueberry8
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.4f, triOffset, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.025f, 0.025f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //Blueberry9
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.38f, -0.05f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 1.0f));
        color = glm::vec4(59.0/255.0, 54.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();


        //Cherry1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.28f, CherryOffset-0.18f, -1.2f)); //ยิ่งใกล้กล้อง ตำแหน่งx,y จะถี่มากขึ้น
        model = glm::scale(model, glm::vec3(0.04f, 0.04f, 1.0f));
        color = glm::vec4(101.0/255.0, 19.0/255.0, 27.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //Cherry2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.33f, CherryOffset+0.23f, -1.2f)); //ยิ่งใกล้กล้อง ตำแหน่งx,y จะถี่มากขึ้น
        model = glm::scale(model, glm::vec3(0.04f, 0.04f, 1.0f));
        color = glm::vec4(101.0/255.0, 19.0/255.0, 27.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();


        //butter1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.001f, 0.231f, -1.2f));
        model = glm::rotate(model, 20 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.07f, 0.07f, 1.0f));
        color = glm::vec4(241.0/255.0, 235.0/255.0, 176.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.23f, -1.2f));
        model = glm::rotate(model, 20 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.08f, 0.08f, 1.0f));
        color = glm::vec4(240.0/255.0, 213.0/255.0, 86.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.15f, 0.18f, -1.2f));
        model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.03f, 1.0f));
        color = glm::vec4(241.0/255.0, 169.0/255.0, 32.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.12f, 0.23f, -1.2f));
        model = glm::rotate(model, -20 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.04f, 1.0f));
        color = glm::vec4(239.0/255.0, 175.0/255.0, 41.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter5
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.07f, 0.23f, -1.2f));
        model = glm::rotate(model, -20 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.08f, 0.05f, 1.0f));
        color = glm::vec4(212.0/255.0, 127.0/255.0, 20.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter6
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.07f, 0.12f, -1.2f));
        model = glm::rotate(model, 30 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.04f, 1.0f));
        color = glm::vec4(240.0/255.0, 175.0/255.0, 42.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter7
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.09f, 0.08f, -1.2f));
        model = glm::rotate(model, 31 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.04f, 1.0f));
        color = glm::vec4(243.0/255.0, 161.0/255.0, 20.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter8
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.06f, 0.11f, -1.2f));
        model = glm::rotate(model, 29 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(204.0/255.0, 108.0/255.0, 24.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter9
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.25f, -0.02f, -1.2f));
        model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.04f, 0.04f, 1.0f));
        color = glm::vec4(243.0/255.0, 167.0/255.0, 28.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter10
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.09f, -0.035f, -1.2f));
        model = glm::rotate(model, 150 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.02f, 0.04f, 1.0f));
        color = glm::vec4(238.0/255.0, 200.0/255.0, 41.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter11
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.09f, -0.15f, -1.2f));
        model = glm::rotate(model, -55 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.04f, 0.05f, 1.0f));
        color = glm::vec4(242.0/255.0, 155.0/255.0, 20.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter12
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.13f, -0.05f, -1.2f));
        model = glm::scale(model, glm::vec3(0.04f, 0.06f, 1.0f));
        color = glm::vec4(239.0/255.0, 209.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter13
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.03f, -0.13f, -1.2f));
        model = glm::scale(model, glm::vec3(0.02f, 0.04f, 1.0f));
        color = glm::vec4(195.0/255.0, 102.0/255.0, 23.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter14
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.07f, -0.1f, -1.2f));
        model = glm::rotate(model, 1550 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.05f, 1.0f));
        color = glm::vec4(239.0/255.0, 209.0/255.0, 68.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter15
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.07f, -0.33f, -1.2f));
        model = glm::rotate(model, -55 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.04f, 0.06f, 1.0f));
        color = glm::vec4(242.0/255.0, 155.0/255.0, 20.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter16
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.3f, -1.2f));
        model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.06f, 0.06f, 1.0f));
        color = glm::vec4(200.0/255.0, 106.0/255.0, 23.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter17
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.09f, -0.25f, -1.2f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(243.0/255.0, 216.0/255.0, 84.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter18
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.1f, -0.3f, -1.2f)); //ยิ่งใกล้กล้อง ตำแหน่งx,y จะถี่มากขึ้น
        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.04f, 1.0f));
        color = glm::vec4(243.0/255.0, 216.0/255.0, 84.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[1]->RenderMesh();
        //butter19
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.1f, -0.25f, -1.2f));
        model = glm::rotate(model, 1550 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.03f, 0.07f, 1.0f));
        color = glm::vec4(243.0/255.0, 216.0/255.0, 84.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter20
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.18f, -0.25f, -1.2f));
        model = glm::rotate(model, 10 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(243.0/255.0, 216.0/255.0, 84.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter21
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.17f, -0.26f, -1.2f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(241.0/255.0, 228.0/255.0, 133.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        //butter22
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.3f, -0.15f, -1.2f));
        model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(242.0/255.0, 155.0/255.0, 20.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //butter23
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.27f, -0.2f, -1.2f)); 
        model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(242.0/255.0, 155.0/255.0, 20.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        
        //butter24
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.21f, -0.15f, -1.2f)); 
        model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(242.0/255.0, 155.0/255.0, 20.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //triHoneySauce1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.25f, -0.05f, -1.2f)); 
        model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.015f, 0.04f, 1.0f));
        color = glm::vec4(186.0/255.0, 123.0/255.0, 53.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[1]->RenderMesh();

        //triHoneySauce1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.31f, -0.01f, -1.2f)); 
        model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.008f, 0.15f, 1.0f));
        color = glm::vec4(186.0/255.0, 123.0/255.0, 53.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[1]->RenderMesh();

        //honey sauce1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.2f, 0.1f, -1.2f)); 
        model = glm::rotate(model, 30 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.15f, 0.09, 1.0f));
        color = glm::vec4(217.0/255.0, 155.0/255.0, 97.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //honey sauce2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.1f, 0.15f, -1.2f)); 
        model = glm::rotate(model, 30 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.25f, 0.15f, 1.0f));
        color = glm::vec4(186.0/255.0, 123.0/255.0, 53.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.04f, 0.17f, -1.2f)); 
        model = glm::rotate(model, 15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.34f, 0.2f, 1.0f));
        color = glm::vec4(217.0/255.0, 155.0/255.0, 97.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.15f, 0.06f, -1.2f)); 
        model = glm::rotate(model, 5 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.25f, 0.18f, 1.0f));
        color = glm::vec4(197.0/255.0, 144.0/255.0, 87.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.15f, -0.03f, -1.2f)); 
        model = glm::rotate(model, -5 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.21f, 0.1f, 1.0f));
        color = glm::vec4(240.0/255.0, 220.0/255.0, 160.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2f, -0.08f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.15f, 0.08f, 1.0f));
        color = glm::vec4(210.0/255.0, 131.0/255.0, 48.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake5
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.14f, 0.05f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.23f, 0.12f, 1.0f));
        color = glm::vec4(145.0/255.0, 72.0/255.0, 35.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake6
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.14f, 0.0f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.2f, 0.1f, 1.0f));
        color = glm::vec4(183.0/255.0, 128.0/255.0, 59.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake7
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.13f, -0.1f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.15f, 0.09f, 1.0f));
        color = glm::vec4(159.0/255.0, 75.0/255.0, 43.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake8
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.08f, -0.15f, -1.2f)); 
        model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.17f, 0.1f, 1.0f));
        color = glm::vec4(176.0/255.0, 134.0/255.0, 72.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pancake9
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.1f, -0.15f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.09f, 0.035f, 1.0f));
        color = glm::vec4(210.0/255.0, 131.0/255.0, 48.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //honey
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.08f, -1.2f)); 
        model = glm::scale(model, glm::vec3(honeyOffset, 0.15f, 1.0f));
        color = glm::vec4(186.0/255.0, 123.0/255.0, 53.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        
        //inner dish
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.4f, 0.3f, 1.0f));
        color = glm::vec4(161.0/255.0, 140.0/255.0, 119.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //dish
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.2f)); 
        model = glm::scale(model, glm::vec3(0.6f, 0.47f, 1.0f));
        color = glm::vec4(244.0/255.0, 239.0/255.0, 232.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //dish edge
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.2f));
        model = glm::scale(model, glm::vec3(0.607f, 0.477f, 1.0f));
        color = glm::vec4(155.0/255.0, 144.0/255.0, 123.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}

/*#undef GLFW_DLL
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <ctime>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

#include "Shader.h"
#include "Window.h"
#include "Mesh.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

bool direction = true;
bool camMoving = true;
float triOffset = -0.15f;
float triMaxOffset = 0.05f;
float triIncrement = 0.3;
int n = 0;

//Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateTriangle()
{

    unsigned int rectIndices[] =
    {
        0, 2, 1,
        0, 3, 2
    };

    GLfloat rectVertices[] =
    {
        -1.0f, -1.0f, 0.0f,   //0
        -1.0f, 1.0f, 0.0f,    //1
        1.0f, 1.0f, 0.0f,    //2
        1.0f, -1.0f, 0.0f     //3
    };

    Mesh* rect = new Mesh();
    rect ->CreateMesh(rectVertices, rectIndices, 12, 6);
    meshList.push_back(rect);

    GLfloat triangleVertices[] =
    {
        -1.0f, -1.0f, 0.0f,     //0
        1.0f, -1.0f, 0.0f,      //1
        0.0f, 1.0f, 0.0f,       //2
    };

    unsigned int triangleIndices[] = 
    {
        0, 1, 2,
    };

    Mesh *triangle = new Mesh();
    triangle->CreateMesh(triangleVertices, triangleIndices, 9, 3);
    meshList.push_back(triangle);

    unsigned int circleIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5
    };

    GLfloat circleVertices[] =
    {
        0.0f, 0.0f, 0.0f,       //0
        1.0f, 0.0f, 0.0f,       //1
        0.94f, 0.35f, 0.0f,     //2
        0.71f, 0.71f, 0.0f,     //3
        0.35f, 0.94f, 0.0f,     //4
        0.0f, 1.0f, 0.0f        //5
    };

    Mesh* circle = new Mesh();
    circle ->CreateMesh(circleVertices, circleIndices, 18, 12);
    meshList.push_back(circle);


}

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

void Update (long elapsedTime){
    if (camMoving){
        if (direction) {
        // s += v*dt
            triOffset += (triIncrement * elapsedTime / 1000.0);
            
        }

        else
        {    triOffset -= (triIncrement * elapsedTime / 1000.0);
        
        }

        if (abs(triOffset) >= triMaxOffset || triOffset == 0.0f) 
        {
            direction = !direction;
            camMoving = false;
            n++;
        }
    }
    /*if (triOffset == 0.0){
        camMoving = false;
        n++;
    }*/
//}
    


/*int main()
{
    mainWindow = Window(WIDTH, HEIGHT);
    mainWindow.initialise();

    CreateTriangle();
    CreateShaders();

    GLuint uniformModel = 0, uniformProjection = 0, uniformColor = 0;
    glm::vec4 color;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

    //statring animation time (before start)
    auto startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    //current animation time (present loop time)
    long currentTime = startTime;
    //last frame animation time (last loop time)
    long lastTime = currentTime;
    //elapsed time (time of last and present loop)
    long elapsedTime;*/

    //glfwSetKeyCallback(mainWindow.getWindow(), keyboardHandler);
    //glfwSetKeyCallback(mainWindow.getWindow());

    //Loop until window closed
    /*while (!mainWindow.getShouldClose())
    {
        currentTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        elapsedTime = currentTime - lastTime;

        lastTime = currentTime;*/

        //Get + Handle user input events
        //glfwPollEvents();

        //Update(elapsedTime);
        //if (n>=5) camMoving = true;
        //if(triOffset < triMaxOffset) triOffset += triIncrement ;

        //Clear window
        /*glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

        //draw here
        /*shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformColor = shaderList[0].GetColorLocation();
        glm::mat4 model (1.0f);*/

        //phone
        //left header
        /*model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.47f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.03f, 0.25f, 1.0f));
        color = glm::vec4(215.0/255.0, 210.0/255.0, 217.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();*/

        //shot inside
        /*model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.55f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.04f, 0.04f, 1.0f));
        color = glm::vec4(1, 1, 1, 1);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //stroke shot inside
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.55f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.045f, 0.045f, 1.0f));
        color = glm::vec4(85.0/255.0, 85.0/255.0, 85.0/255.0, 1);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //shot button
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.55f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(1, 1, 1, 1);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //right upper
        //green circle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.58f, 0.2f, -1.0f));
        model = glm::scale (model, glm::vec3(0.03f, 0.03f, 1.0f));
        color = glm::vec4(15.0/255.0, 255.0/255.0, 15.0/255.0, 1);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        //red circle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.555f, 0.23f, -1.0f));
        model = glm::scale (model, glm::vec3(0.03f, 0.03f, 1.0f));
        color = glm::vec4(255.0/255.0, 15.0/255.0, 15.0/255.0, 1);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //blue circle
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.535f, 0.2f, -1.0f));
        model = glm::scale (model, glm::vec3(0.03f, 0.03f, 1.0f));
        color = glm::vec4(15.0/255.0, 15.0/255.0, 255.0/255.0, 1);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //pic albums
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.55f, -0.22f, -1.0f));
        model = glm::scale (model, glm::vec3(0.045f, 0.045f, 1.0f));
        color = glm::vec4(210.0/255.0, 239.0/255.0, 201.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();

        //right
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.55f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.085f, 0.3f, 1.0f));
        color = glm::vec4(185.0/255.0, 185.0/255.0, 185.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();
        
        //screen
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.6f, 0.3f, 1.0f));
        color = glm::vec4(112.0/255.0, 112.0/255.0, 114.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();

        //camera
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.65f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.01f, 0.01f, 1.0f));
        color = glm::vec4(150.0/255.0, 150.0/255.0, 150.0/255.0, 1);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //nail finger 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.6f, -0.45f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(255.0/255.0, 230.0/255.0, 204.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.6f, -0.4f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(255.0/255.0, 230.0/255.0, 204.0/255.0, 1);
        model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //finger 1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.6f, -0.4f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.68f, -0.4f, -1.0f));
        model = glm::scale (model, glm::vec3(0.08f, 0.05f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        //model = glm::rotate(model, 10 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();*/


        // border phone
        /*Update(elapsedTime);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, triOffset, -1.0f));
        model = glm::scale (model, glm::vec3(0.7f, 0.35f, 1.0f));
        color = glm::vec4(37.0/255.0, 37.0/255.0, 37.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();

        //nail finger 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.6f, 0.4f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(255.0/255.0, 230.0/255.0, 204.0/255.0, 1);
        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.6f, 0.45f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(255.0/255.0, 230.0/255.0, 204.0/255.0, 1);
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //finger 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.6f, 0.4f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.65f, 0.4f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, 0.4f, -1.0f));
        model = glm::scale (model, glm::vec3(0.05f, 0.05f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, 0.35f, -1.0f));
        model = glm::scale (model, glm::vec3(0.06f, 0.06f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        model = glm::rotate(model, -50 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[2]->RenderMesh();

        //finger 3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, 0.2f, -1.0f));
        model = glm::scale (model, glm::vec3(0.08f, 0.06f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();

        //finger 4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, 0.0f, -1.0f));
        model = glm::scale (model, glm::vec3(0.08f, 0.05f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        model = glm::rotate(model, -15 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();

        //finger 5
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, -0.2f, -1.0f));
        model = glm::scale (model, glm::vec3(0.08f, 0.05f, 1.0f));
        color = glm::vec4(252.0/255.0, 190.0/255.0, 122.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();*/

    
        //test bg
        /*model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.2f));
        model = glm::scale (model, glm::vec3(0.6f, 0.6f, 1.0f));
        color = glm::vec4(138.0/255.0, 253.0/255.0, 62.0/255.0, 1);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform4fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();


        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}*/
