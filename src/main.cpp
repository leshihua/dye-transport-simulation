#include <algorithm>
#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "src/graphics/interface.h"

// GLFW function prototypes
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void resizeCallback(GLFWwindow *window, GLint width, GLint height);

// Grid and window dimensions
const GLint WIDTH = 120, HEIGHT = 120;
const GLint ZOOM = 4;

Interface interface(WIDTH, HEIGHT);

// Boilerplate starter code from CS 148 (Summer 2016) Assignment 3's starter code.
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH * ZOOM, HEIGHT * ZOOM,
                                          "Dye Transport Simulator", nullptr, nullptr);
    glfwSetWindowAspectRatio(window, WIDTH, HEIGHT);
    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    glGetError(); // Called once to catch any glewInit() bugs

    // Set the required callback functions
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowSizeCallback(window, resizeCallback);

    // OpenGL configuration
    glViewport(0, 0, WIDTH * ZOOM, HEIGHT * ZOOM);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // DeltaTime variables
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    // Initialize interface
    interface.init();
    interface.state = INTERFACE_ACTIVE;

    double lastCheckpoint = glfwGetTime();
    int renderedFrames = 0;

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        ++renderedFrames;
        if (currentFrame - lastCheckpoint >= 1) {
            std::cout << 1000.0 / renderedFrames << " ms/frame" << std::endl;
            renderedFrames = 0;
            lastCheckpoint = currentFrame;
        }

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Manage user input and game state
        interface.processInput(deltaTime);
        interface.update(deltaTime);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        interface.render();

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    ResourceManager::clear();
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            interface.keys[key] = GL_TRUE;
            interface.keysDown[key] = GL_TRUE;
            interface.keysUp[key] = GL_FALSE;
        } else if (action == GLFW_RELEASE) {
            interface.keys[key] = GL_FALSE;
            interface.keysUp[key] = GL_TRUE;
            interface.keysDown[key] = GL_FALSE;
        }
    }
}

// Is called whenever the window is resized via GLFW
void resizeCallback(GLFWwindow* window, GLint width, GLint height)
{
    interface.processResize(width, height);
    glViewport(0, 0, width, height);
}
