//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
// Modified by Ben Marcotte for COMP371 Assignment 1

#include <iostream>
#include <list>
#include <glm/gtx/string_cast.hpp>
#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>


using namespace glm;
using namespace std;


const char* getVertexShaderSource()
{
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        ""
        "uniform mat4 worldMatrix = mat4(1.0);"
        "uniform mat4 viewMatrix = mat4(1.0);"
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


int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
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
    const char* fragmentShaderSource = getFragmentShaderSource();
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


int createVertexBufferObject()
{
    // Cube model -- taken from lab, added unit lines at bottom
    vec3 vertexArray[] = {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), //left
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), // far
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // bottom 
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // near 
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // right 
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // top 
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),


        

        vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), // line in x
        vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), // line in y
        vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f), // line in z
        vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 1.0f, 0.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), // redline in x
        vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), // blueline in y
        vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),

        vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), // greenline in z
        vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f),



        //  BLACK FOR NOSE
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f), //left
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f), // far
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f), // bottom 
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f), // near 
        vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f), // right 
        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f), // top 
        vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 0.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 0.0f),

    };


    // Create a vertex array -- taken from lab
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Taken from lab
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(vec3),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);


    return vertexBufferObject;
}


int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Taken from lab, modified values to match assignment parameters
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Comp371 - Assignment 1 - 40122097", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Changed values to sort of match green from assignment
    glClearColor(0.0f, 0.2f, 0.1f, 1.0f);

    // Compile and link shaders here ... -- taken from lab
    int shaderProgram = compileAndLinkShaders();

    // We can set the shader once, since we have only one -- taken from lab
    glUseProgram(shaderProgram);


    // Camera parameters for view transform -- taken from lab with modified values
    vec3 cameraPosition(-2.f, -2.0f, 2.0f);
    vec3 cameraLookAt(1.0f, 1.0f, 2.0f);
    vec3 cameraUp(0.0f, 0.0f, 10.0f);

    // Other camera parameters -- taken from lab
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 45.0f;
    float cameraVerticalAngle = 0.0f;

    mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
        800.0f / 600.0f,  // aspect ratio
        0.01f, 100.0f);   // near and far (near > 0)

    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    mat4 worldMatrix = mat4(1.0);

    GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
    glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);



    // Define and upload geometry to the GPU here ...
    int vao = createVertexBufferObject();

    // For frame time
    float lastFrameTime = glfwGetTime();

    // Enable Backface culling
    glEnable(GL_CULL_FACE);

    // @TODO 1 - Enable Depth Test
    glEnable(GL_DEPTH_TEST);

    //render mode for olaf, default triangles
    char renderMode = GL_TRIANGLES;

    //olaf init position
    mat4 olafWorldMatrix = translate(mat4(1.0f), vec3(10.0f, 10.0f, 0.0f)) * scale(mat4(1.0f), vec3(3.0f, 3.0f, 3.0f));

    //prevent teleporting/resizing every frame
    uint framesSinceLastTP = 0;
    uint framesSinceLastSize = 0;

    double xmouse, ymouse, pxmouse, pymouse, dx, dy;

    glfwGetCursorPos(window, &xmouse, &ymouse);

    float fov = 70.0f;


    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        framesSinceLastTP++;
        framesSinceLastSize++;

        pxmouse = xmouse;
        pymouse = ymouse;
        
        glfwGetCursorPos(window, &xmouse, &ymouse);

        dx = xmouse - pxmouse;
        dy = ymouse - pymouse;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);

        //GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");
        mat4 groundWorldMatrix;

        //Drawing coord lines
        groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_LINES, 42, 2);

        groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 5.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_LINES, 44, 2);

        groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(1.0f, 1.0f, 5.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        glDrawArrays(GL_LINES, 46, 2);


        //Drawing floor grid
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
        for (int i = 0; i <= 100; ++i)
        {
            groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f, -50.0f + i * 1.0f, -2.0f)) * scale(mat4(1.0f), vec3(100.0f, 1.0f, 1.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_LINES, 36, 2);

            groundWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + i*1.0f, -50.0f, -2.0f)) * scale(mat4(1.0f), vec3(1.0f, 100.0f, 1.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &groundWorldMatrix[0][0]);
            glDrawArrays(GL_LINES, 38, 2);
            
        }


        // renderMode: triangle, point or line
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
            renderMode = GL_TRIANGLES;
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
            renderMode = GL_POINTS;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            renderMode = GL_LINES;

        // Draw Olaf
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olafWorldMatrix[0][0]);
        glDrawArrays(renderMode, 0, 36);
        mat4 olaf2 = olafWorldMatrix;
        mat4 olaf3 = olafWorldMatrix;

        olaf2 = translate(olaf2, vec3(0.05f, 0.05f, 0.75f)) * scale(mat4(1.0), vec3(0.6f, 0.6f, 0.6f));
        olaf3 = translate(olaf3, vec3(0.05f, 0.05f, 1.2f)) * scale(mat4(1.0), vec3(0.3f, 0.3f, 0.3f));
        mat4 nose = olaf3;
        nose = translate(nose, vec3(0.4f, 0.0f, 0.15f)) * scale(mat4(1.0), vec3(0.8f, 0.4f, 0.4f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olaf2[0][0]);
        glDrawArrays(renderMode, 0, 36);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &olaf3[0][0]);
        glDrawArrays(renderMode, 0, 36);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &nose[0][0]);
        glDrawArrays(renderMode, 48, 36);




        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);


        const float cameraAngularSpeed = 60.0f;

        float phi = radians(cameraHorizontalAngle);
        float theta = radians(cameraVerticalAngle);

        if (theta > 0.78f)
            theta = 0.78f;
        if (theta < -0.78f)
            theta = -0.78f;

        cameraLookAt = vec3(cosf(phi), sinf(phi), sinf(theta)); //math figured out kind of by trial and error -- modified from lab


        //making normalized direction vector for camera movement
        vec3 direction = cameraLookAt;

        glm::normalize(direction);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move olaf to the left
        {
            olafWorldMatrix = translate(olafWorldMatrix, vec3(0.0f, -0.1f, 0.0f));

            //olafWorldMatrix = translate(mat4(1.0f), vec3(,,));
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move olaf to the right
        {
            olafWorldMatrix = translate(olafWorldMatrix, vec3(0.0f, 0.1f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move olaf backward
        {
            olafWorldMatrix = translate(olafWorldMatrix, vec3(-0.1f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move olaf forward
        {

            olafWorldMatrix = translate(olafWorldMatrix, vec3(0.1f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // rotate olaf left
        {
            olafWorldMatrix = rotate(olafWorldMatrix, 0.1f, vec3(0.0f,0.0f,1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // rotate olaf right
        {
            olafWorldMatrix = rotate(olafWorldMatrix, -0.1f, vec3(0.0f, 0.0f, 1.0f));
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) // rotate camera when right mouse is pressed
        {
            cameraHorizontalAngle += cameraAngularSpeed/4.0f * dx * dt;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) // rotate camera when right mouse is pressed
        {
            cameraVerticalAngle += cameraAngularSpeed / 4.0f * dy * dt;
        }


        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) // rotate camera when right mouse is pressed
        {
            fov += dy * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && framesSinceLastTP > 25) // teleport olaf to random position
        {
            olafWorldMatrix = translate(mat4(1.0f), vec3(-50.0f + (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0f))), 
                -50.0f + (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0f))),
                                                                                                                           0.0f));
            framesSinceLastTP = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && framesSinceLastSize > 5) // scale olaf up
        {
            olafWorldMatrix *= scale(mat4(1.0f), vec3(1.05f, 1.05f, 1.05f));
            framesSinceLastSize = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && framesSinceLastSize > 5) // scale olaf down
        {
            olafWorldMatrix *= scale(mat4(1.0f), vec3(0.95f, 0.95f, 0.95f));
            framesSinceLastSize = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) // reset world
        {
            worldMatrix = mat4(1.0f);
        }


        //for some reason, these dont work despite following the same process as the view/projection matrices
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // rotate world R-x
        {
            worldMatrix = rotate(worldMatrix, -radians(dt*cameraAngularSpeed), vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // rotate world Rx
        {
            worldMatrix = rotate(worldMatrix, radians(dt * cameraAngularSpeed), vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // rotate world Ry
        {
            worldMatrix = rotate(worldMatrix, radians(dt * cameraAngularSpeed), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // rotate world R-y
        {
            worldMatrix = rotate(worldMatrix, -radians(dt * cameraAngularSpeed), vec3(0.0f, 1.0f, 0.0f));
        }

        if (fov < 69.5f)
            fov = 69.5f;
        if (fov > 71.0f)
            fov = 71.0f;

        projectionMatrix = glm::perspective(fov,            // field of view in degrees
            800.0f / 600.0f,  // aspect ratio
            0.01f, 100.0f);   // near and far (near > 0)

        //Taken from lab, modified for new coordinates
        mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        cout << glm::to_string(worldMatrix) << endl;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

    }


    // Shutdown GLFW
    glfwTerminate();

    return 0;
}
