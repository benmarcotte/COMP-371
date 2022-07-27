//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//
// Inspired by the following tutorials:
// - https://learnopengl.com/Getting-started/Hello-Window
// - https://learnopengl.com/Getting-started/Hello-Triangle

#include <iostream>
#include <list>
#include <algorithm>
#include <math.h>
#include <ctime> 
#include <vector> 
#include <fstream>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <FreeImageIO.h>


using namespace std;
using namespace glm;

// window dimensions
const GLuint WIDTH = 1024, HEIGHT = 768;

float drawGrid = true;

// Camera parameters
float camera_fov = 70.0f;
float dt = 0.0f;

// Camera parameters for view transform -- taken from lab 
vec3 cameraPosition(0.6f, 1.0f, 20.0f);
vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);
vec3 cameraSideVector = cross(cameraLookAt, cameraUp);

// Camera movement
float cameraRotateX = 0.0f;
float cameraRotateY = 0.0f;
float cameraRotationSpeed = 5.0f;
float cameraRotationAmount = 1.0f;

float cameraPanningSpeed = 1.0f;
float cameraTiltingSpeed = 1.0f;
float cameraFovChangeSpeed = 0.01f;

// Cursor position
double lastMousePosX;
double lastMousePosY;

// Olaf parameters
bool moveForwardAndBack = false;
bool moveLeftRight = false;

// Lab
GLuint loadTexture(const char* filename);

const char* getVertexShaderSource();
const char* getFragmentShaderSource();
const char* getTexturedVertexShaderSource();
const char* getTexturedFragmentShaderSource();
int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource);

struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
        : position(_position), color(_color), uv(_uv) {}

    vec3 position;
    vec3 color;
    vec2 uv;
};

int createLineVertexArrayObject()
{
    // A vertex is a point on a polygon, it contains positions and other data (eg: colors)
    vec3 vertexArray[] = {
        // Position              Color
        vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), // line in x
        vec3(5.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), // line in y
        vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), // line in z
        vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 1.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), // gridline in x
        vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), // gridline in z
        vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f)
    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0 ,3,GL_FLOAT, GL_FALSE, 2 * sizeof(vec3),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    return vertexArrayObject;
}

int createCubeVertexArrayObject(vec3 color)
{
    vec3 vertexArray[] = {
        // Position              Color
        vec3(-0.5f,-0.5f,-0.5f), color,
        vec3(-0.5f,-0.5f, 0.5f), color,
        vec3(-0.5f, 0.5f, 0.5f), color,

        vec3(-0.5f,-0.5f,-0.5f), color,
        vec3(-0.5f, 0.5f, 0.5f), color,
        vec3(-0.5f, 0.5f,-0.5f), color,

        vec3( 0.5f, 0.5f,-0.5f), color,
        vec3(-0.5f,-0.5f,-0.5f), color,
        vec3(-0.5f, 0.5f,-0.5f), color,

        vec3( 0.5f, 0.5f,-0.5f), color,
        vec3( 0.5f,-0.5f,-0.5f), color,
        vec3(-0.5f,-0.5f,-0.5f),color,

        vec3( 0.5f,-0.5f, 0.5f), color,
        vec3(-0.5f,-0.5f,-0.5f), color,
        vec3( 0.5f,-0.5f,-0.5f), color,

        vec3( 0.5f,-0.5f, 0.5f), color,
        vec3(-0.5f,-0.5f, 0.5f), color,
        vec3(-0.5f,-0.5f,-0.5f), color,

        vec3(-0.5f, 0.5f, 0.5f), color,
        vec3(-0.5f,-0.5f, 0.5f), color,
        vec3( 0.5f,-0.5f, 0.5f), color,

        vec3( 0.5f, 0.5f, 0.5f), color,
        vec3(-0.5f, 0.5f, 0.5f), color,
        vec3( 0.5f,-0.5f, 0.5f), color,

        vec3( 0.5f, 0.5f, 0.5f), color,
        vec3( 0.5f,-0.5f,-0.5f), color,
        vec3( 0.5f, 0.5f,-0.5f), color,

        vec3( 0.5f,-0.5f,-0.5f), color,
        vec3( 0.5f, 0.5f, 0.5f), color,
        vec3( 0.5f,-0.5f, 0.5f), color,

        vec3( 0.5f, 0.5f, 0.5f), color,
        vec3( 0.5f, 0.5f,-0.5f), color,
        vec3(-0.5f, 0.5f,-0.5f), color,

        vec3( 0.5f, 0.5f, 0.5f), color,
        vec3(-0.5f, 0.5f,-0.5f), color,
        vec3(-0.5f, 0.5f, 0.5f), color
    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vec3), (void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // VAO already stored the state we just defined, safe to unbind buffer
    glBindVertexArray(0); // Unbind to not modify the VAO

    glBindVertexArray(vertexBufferObject);

    return vertexArrayObject;
}

int createTexturedCubeVertexArrayObject(vec3 color)
{
    const TexturedColoredVertex texturedCubeVertexArray[] = {
        // position,       color        texture   
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), color, vec2(0.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f)),

        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(0.0f, 1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), color, vec2(1.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), color, vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), color, vec2(0.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),

        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(0.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), color, vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(0.0f, 1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), color, vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), color, vec2(1.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f,-0.5f,-0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3( 0.5f,-0.5f, 0.5f), color, vec2(0.0f, 1.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3( 0.5f, 0.5f,-0.5f), color, vec2(1.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(0.0f, 0.0f)),

        TexturedColoredVertex(vec3( 0.5f, 0.5f, 0.5f), color, vec2(1.0f, 1.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), color, vec2(0.0f, 0.0f)),
        TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), color, vec2(0.0f, 1.0f))
    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedColoredVertex),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, sizeof(TexturedColoredVertex),(void*)sizeof(vec3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE,sizeof(TexturedColoredVertex),(void*)(2 * sizeof(vec3)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    glBindVertexArray(vertexBufferObject);

    return vertexArrayObject;
}

float randomFloat(float start, float end) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = end - start;
    float offset = random * diff;
    return start + offset;
}

void setProjectionMatrix(int shaderProgram, mat4 projectionMatrix)
{
    glUseProgram(shaderProgram);
    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
}

void setViewMatrix(int shaderProgram, mat4 viewMatrix)
{
    glUseProgram(shaderProgram);
    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
}

void setWorldMatrix(int shaderProgram, mat4 worldMatrix)
{
    glUseProgram(shaderProgram);
    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
}

int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Comp371 - Assignment 01", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's content current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Load Textures
    GLuint groundTextureID = loadTexture("../assets/textures/snowGround.jpg");
    GLuint hatTextureID = loadTexture("../assets/textures/blackHat.jpg");
    GLuint noseTextureID = loadTexture("../assets/textures/orangeNose.jpg");

    // Black background
    glClearColor(0.8f, 0.9f, 0.9f, 1.0f);

    // Compile and link shaders here ...
    int colorShaderProgram = compileAndLinkShaders(getVertexShaderSource(), getFragmentShaderSource());
    int textureShaderProgram = compileAndLinkShaders(getTexturedVertexShaderSource(), getTexturedFragmentShaderSource());

    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
        800.0f / 600.0f,  // aspect ratio
        0.01f, 100.0f);   // near and far (near > 0)

// Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

// Set View and Projection matrices on both shaders
    setViewMatrix(colorShaderProgram, viewMatrix);
    setViewMatrix(textureShaderProgram, viewMatrix);

    setProjectionMatrix(colorShaderProgram, projectionMatrix);
    setProjectionMatrix(textureShaderProgram, projectionMatrix);

    // Set world matrix
    mat4 worldMatrix = mat4(1.0);
    setWorldMatrix(colorShaderProgram, worldMatrix);
    setWorldMatrix(textureShaderProgram, worldMatrix);

    // Define and upload geometry to the GPU here ...
    int vao = createLineVertexArrayObject();

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    //artifact from assignment
    char renderMode = GL_TRIANGLES;
    float fov = 70.0f;

    // Enable Backface culling
    glEnable(GL_CULL_FACE);

    // Enable Depth Test
    glEnable(GL_DEPTH_TEST);

    // Initial Olaf position
    mat4 olaf = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));

    // Prevent olaf resizing or lost every frame
    uint lastFrameSize = 0;

    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // taken from lab
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        lastFrameSize++;

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind geometry
        glBindVertexArray(vao);
        glUseProgram(colorShaderProgram);

        mat4 groundWorldMatrix;

        // Draw axis lines
        groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
        setWorldMatrix(colorShaderProgram, groundWorldMatrix);
        glDrawArrays(GL_LINES, 0, 2);
        groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
        setWorldMatrix(colorShaderProgram, groundWorldMatrix);
        glDrawArrays(GL_LINES, 2, 2);
        groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f));
        setWorldMatrix(colorShaderProgram, groundWorldMatrix);
        glDrawArrays(GL_LINES, 4, 2);

        //Drawing floor grid
        mat4 gridWorldMatrix = mat4(1.0f);;
        if (drawGrid)
        {
            for (int i = 0; i <= 100; ++i)
            {
                gridWorldMatrix = translate(mat4(1.0f), vec3(-50.0f, -0.1f, -50.0f + i * 1.0f)) * scale(mat4(1.0f), vec3(100.0f, 1.0f, 1.0f));
                setWorldMatrix(colorShaderProgram, gridWorldMatrix);
                glDrawArrays(GL_LINES, 6, 2);

                gridWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i * 1.0f, -0.1f, -50.0f)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 100.0f));
                setWorldMatrix(colorShaderProgram, gridWorldMatrix);
                glDrawArrays(GL_LINES, 8, 2);
            }
        }

        // Draw ground
        glUseProgram(textureShaderProgram);
        createTexturedCubeVertexArrayObject(vec3(1.0f, 1.0f, 1.0f));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTextureID);


        groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, -0.3f, 0.0f)) * scale(mat4(1.0f), vec3(100.0f, 0.1f, 100.0f));
        setWorldMatrix(textureShaderProgram, groundWorldMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // renderMode: triangle, point or line
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
            renderMode = GL_TRIANGLES;
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
            renderMode = GL_POINTS;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            renderMode = GL_LINES;

        // Draw olaf 
        glUseProgram(colorShaderProgram);
        createCubeVertexArrayObject(vec3(0.85f, 0.85f, 0.85f));
        setWorldMatrix(colorShaderProgram, olaf);

            float olafLimbRotationAmount = 2.5f;

        float footAngle = 0.0f;
        float footDistanceFromBase = 1.0f;
        if (moveForwardAndBack)
        {
            footAngle = (int)(25 * sin(lastFrameTime * 5)) % 75;
        }
        else if (moveLeftRight)
        {
            footAngle = (int)(25 * sin(lastFrameTime * 3)) % 75;
        }
        // Olaf left leg
        mat4 leftLeg_olaf = olaf;
        leftLeg_olaf = translate(olaf, vec3(-0.5f, 0.25f, 0.0f)) * scale(mat4(1.0), vec3(0.5f, 1.0f, 0.5f));

        if (moveForwardAndBack)
        {
            leftLeg_olaf = rotate(leftLeg_olaf, radians(footAngle), vec3(1.0f, 0.0f, 0.0f));
        }
        else if (moveLeftRight)
        {
            leftLeg_olaf = rotate(leftLeg_olaf, radians(footAngle), vec3(0.0f, 0.0f, 1.0f));
        }


        setWorldMatrix(colorShaderProgram, leftLeg_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf right leg
        mat4 rightLeg_olaf = olaf;
        rightLeg_olaf = translate(rightLeg_olaf, vec3(0.5f, 0.25f, 0.0f)) * scale(mat4(1.0), vec3(0.5f, 1.0f, 0.5f));

        if (moveForwardAndBack)
        {
            rightLeg_olaf = rotate(rightLeg_olaf, radians(-footAngle), vec3(1.0f, 0.0f, 0.0f));
        }
        else if (moveLeftRight)
        {
            rightLeg_olaf = rotate(rightLeg_olaf, radians(footAngle), vec3(0.0f, 0.0f, 1.0f));
        }

        setWorldMatrix(colorShaderProgram, rightLeg_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf body
        mat4 body_olaf = olaf;
        body_olaf = translate(body_olaf, vec3(0.0f, 1.25f, 0.0f)) * scale(mat4(1.0), vec3(2.0f, 1.5f, 1.0f));
        setWorldMatrix(colorShaderProgram, body_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf torso
        mat4 torso_olaf = olaf;
        torso_olaf = translate(torso_olaf, vec3(0.0f, 2.25f, 0.0f)) * scale(mat4(1.0), vec3(1.5f, 0.5f, 1.0f));
        setWorldMatrix(colorShaderProgram, torso_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf left arm
        mat4 leftArm_olaf = olaf;
        leftArm_olaf = translate(leftArm_olaf, vec3(-1.5f, 2.25f, 0.0f)) * scale(mat4(1.0), vec3(1.5f, 0.20f, 0.5f));
        setWorldMatrix(colorShaderProgram, leftArm_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf right arm
        mat4 rightArm_olaf = olaf;
        rightArm_olaf = translate(rightArm_olaf, vec3(1.5f, 2.25f, 0.0f)) * scale(mat4(1.0), vec3(1.5f, 0.20f, 0.5f));
        setWorldMatrix(colorShaderProgram, rightArm_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf head
        mat4 head_olaf = olaf;
        head_olaf = translate(head_olaf, vec3(0.0f, 2.9f, 0.0f)) * scale(mat4(1.0), vec3(1.0f, 1.0f, 1.0f));
        setWorldMatrix(colorShaderProgram, head_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Draw olaf face features
        // Olaf nose
        glUseProgram(textureShaderProgram);
        createTexturedCubeVertexArrayObject(vec3(0.9f, 0.6f, 0.4f));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, noseTextureID);

        mat4 nose_olaf = olaf;
        nose_olaf = translate(nose_olaf, vec3(0.0f, 3.0f, 0.6f)) * scale(mat4(1.0), vec3(0.1f, 0.1f, 0.5f));
        setWorldMatrix(textureShaderProgram, nose_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf mouth, eyes, hair
        glUseProgram(colorShaderProgram);
        createCubeVertexArrayObject(vec3(0.0f, 0.0f, 0.0f));

        // Olaf mouth
        mat4 mouth_olaf = olaf;
        mouth_olaf = translate(mouth_olaf, vec3(0.0f, 2.8f, 0.5f)) * scale(mat4(1.0), vec3(0.20f, 0.05f, 0.1f));
        setWorldMatrix(colorShaderProgram, mouth_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf left eye
        mat4 leftEye_olaf = olaf;
        leftEye_olaf = translate(leftEye_olaf, vec3(-0.25f, 3.2f, 0.5f)) * scale(mat4(1.0), vec3(0.20f, 0.2f, 0.1f));
        setWorldMatrix(colorShaderProgram, leftEye_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf right eye
        mat4 rightEye_olaf = olaf;
        rightEye_olaf = translate(rightEye_olaf, vec3(0.25f, 3.2f, 0.5f)) * scale(mat4(1.0), vec3(0.20f, 0.2f, 0.1f));
        setWorldMatrix(colorShaderProgram, rightEye_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf top button
        glUseProgram(textureShaderProgram);
        createTexturedCubeVertexArrayObject(vec3(0.0f, 0.0f, 0.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hatTextureID);

        mat4 topButton_olaf = olaf;
        topButton_olaf = translate(topButton_olaf, vec3(0.0f, 1.8f, 0.5f)) * scale(mat4(1.0), vec3(0.20f, 0.2f, 0.2f));
        setWorldMatrix(textureShaderProgram, topButton_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf middle button
        mat4 midButton_olaf = olaf;
        midButton_olaf = translate(midButton_olaf, vec3(0.0f, 1.4f, 0.5f)) * scale(mat4(1.0), vec3(0.20f, 0.2f, 0.2f));
        setWorldMatrix(textureShaderProgram, midButton_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf bottom button
        mat4 botButton_olaf = olaf;
        botButton_olaf = translate(botButton_olaf, vec3(0.0f, 1.0f, 0.5f)) * scale(mat4(1.0), vec3(0.20f, 0.2f, 0.2f));
        setWorldMatrix(textureShaderProgram, botButton_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf base hat
        mat4 baseHat_olaf = olaf;
        baseHat_olaf = translate(baseHat_olaf, vec3(0.0f, 3.4f, 0.0f)) * scale(mat4(1.0), vec3(1.2f, 0.1f, 1.2f));
        setWorldMatrix(textureShaderProgram, baseHat_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf top hat
        mat4 topHat_olaf = olaf;
        topHat_olaf = translate(topHat_olaf, vec3(0.0f, 3.6f, 0.0f)) * scale(mat4(1.0), vec3(0.8f, 0.7f, 0.8f));
        setWorldMatrix(textureShaderProgram, topHat_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Olaf scarf
        glUseProgram(colorShaderProgram);
        createCubeVertexArrayObject(vec3(0.65f, 0.15f, 0.15f));
        mat4 scarf_olaf = olaf;
        scarf_olaf = translate(scarf_olaf, vec3(0.0f, 2.6f, 0.0f)) * scale(mat4(1.0), vec3(1.1f, 0.15f, 1.1f));
        setWorldMatrix(colorShaderProgram, scarf_olaf);
        glDrawArrays(renderMode, 0, 36);

        // Draw colored geometry
        glUseProgram(colorShaderProgram);

        // End frame
        glfwSwapBuffers(window);

        // Detect inputs
        glfwPollEvents();

        // Close window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Display grid
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            drawGrid = false;
        }


        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move left
        {
            olaf = translate(olaf, vec3(-0.1f, 0.0f, 0.0f));
            moveLeftRight = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move right
        {
            olaf = translate(olaf, vec3(0.1f, 0.0f, 0.0f));
            moveLeftRight = true;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move back
        {
            olaf = translate(olaf, vec3(0.0f, 0.0f, -0.1f));
            moveForwardAndBack = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move forth
        {
            olaf = translate(olaf, vec3(0.0f, 0.0f, 0.1f));
            moveForwardAndBack = true;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // rotate left
        {
            olaf = rotate(olaf, 0.1f, vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // rotate right
        {
            olaf = rotate(olaf, -0.1f, vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && lastFrameSize > 5) // scale olaf up
        {
            olaf *= scale(mat4(1.0f), vec3(1.05f, 1.05f, 1.05f));
            lastFrameSize = 0;
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && lastFrameSize > 5) // scale olaf down
        {
            olaf *= scale(mat4(1.0f), vec3(0.95f, 0.95f, 0.95f));
            lastFrameSize = 0;
        }


        // Moving camera 
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

        // Calculate mouse motion dx and dy
        // pdate camera horizontal and vertical angle
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        // Determine the change in cursor position
        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        // Update previous cursor position values
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Allow panning if right mouse btn is held
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            cameraHorizontalAngle -= dx * cameraPanningSpeed * dt;
        }

        // Allow tilting if middle mouse btn is held
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
            cameraVerticalAngle -= dy * cameraTiltingSpeed * dt;
        }

        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        if (cameraHorizontalAngle > 360)
        {
            cameraHorizontalAngle -= 360;
        }
        else if (cameraHorizontalAngle < -360)
        {
            cameraHorizontalAngle += 360;
        }

        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);

        cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));
        glm::normalize(cameraSideVector);

        // Allow zomming if left mouse btn is held
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            camera_fov += cameraFovChangeSpeed * (float)dy;
        }

        // use camera lookat and side vectors to update positions with ASDW
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // move camera up
        {
            cameraRotateY += cameraRotationAmount;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // move camera down
        {
            cameraRotateY -= cameraRotationAmount;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // move camera to the left
        {
            cameraRotateX -= cameraRotationAmount;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // move camera to the right
        {
            cameraRotateX += cameraRotationAmount;
        }

        //camera reset - lab
        if (glfwGetKey(window, GLFW_KEY_HOME) || glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            camera_fov = 70.0f;

            cameraPosition = vec3(0.6f, 1.0f, 20.0f);
            cameraLookAt = vec3(0.0f, 0.0f, -1.0f);
            cameraUp = vec3(0.0f, 1.0f, 0.0f);

            cameraRotateX = 0.0f;
            cameraRotateY = 0.0f;

            cameraHorizontalAngle = 90.0f;
            cameraVerticalAngle = -0.0f;
        }

        // Set the view matrix for first camera
        mat4 viewMatrix = mat4(1.0);
        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        viewMatrix = rotate(viewMatrix, radians(cameraRotationSpeed * cameraRotateX), cameraUp);

        setViewMatrix(colorShaderProgram, viewMatrix);
        setViewMatrix(textureShaderProgram, viewMatrix);

        if (camera_fov < 69.5f)
            camera_fov = 69.5f;
        if (camera_fov > 71.0f)
            camera_fov = 71.0f;
        mat4 projectionMatrix = perspective(camera_fov, (float)1024 / 768, 0.1f, 100.0f);
        setProjectionMatrix(colorShaderProgram, projectionMatrix);
        setProjectionMatrix(textureShaderProgram, projectionMatrix);

        float cameraRotationAngle = cameraRotationSpeed * (cameraRotateX - 45.0f);
        vec3 cameraRotationPos = vec3(-cos(radians(cameraRotationAngle)), 0.0f, -sin(radians(cameraRotationAngle)));
        viewMatrix = rotate(viewMatrix, radians(cameraRotationSpeed * cameraRotateY), cameraRotationPos);

        setWorldMatrix(colorShaderProgram, worldMatrix);
        setWorldMatrix(textureShaderProgram, worldMatrix);

    }
    // Shutdown GLFW
    glfwTerminate();
    return 0;
}


const char* getVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
}

const char* getFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "in vec3 vertexColor;"
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   FragColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0f);"
        "}";
}

const char* getTexturedVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        "layout (location = 2) in vec2 aUV;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "out vec2 vertexUV;"
        ""
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "   vertexUV = aUV;"
        "}";
}

const char* getTexturedFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "in vec3 vertexColor;"
        "in vec2 vertexUV;"
        "uniform sampler2D textureSampler;"
        "uniform sampler2D textureSampler2;"
        ""
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   vec4 textureColor = texture( textureSampler, vertexUV );"
        "   vec4 textureColor2 = texture( textureSampler2, vertexUV );"
        "   FragColor = textureColor * textureColor2;"
        "}";
}

int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint loadTexture(const char* filename)
{
    // Load image using the Free Image library
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);
    FIBITMAP* image = FreeImage_Load(format, filename);
    FIBITMAP* image32bits = FreeImage_ConvertTo32Bits(image);

    // Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Retrieve width and hight
    int width = FreeImage_GetWidth(image32bits);
    int height = FreeImage_GetHeight(image32bits);

    // This will upload the texture to the GPU memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
        0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(image32bits));

    // Free images
    FreeImage_Unload(image);
    FreeImage_Unload(image32bits);

    return textureId;
}
