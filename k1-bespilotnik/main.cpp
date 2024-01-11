#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "shader_helper.h"
#include "texture_helper.h"
#include "constants.h"
#include "map_drawer.h"
#include "model.hpp"
#include "planes_drawer.h"
#include "shader.hpp"
#include "title_drawer.h"
using namespace std;

int main() {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Init libs
    if (!glfwInit()) {
        cout << "GLFW init fail!" << endl;
        return 1;
    }

    //Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Make Window
    GLFWwindow* window;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        cout << "Window init fail!" << endl;
        return 1;
    }
    glfwMakeContextCurrent(window); //Window to be active window

    if (glewInit() != GLEW_OK) {
        cout << "GLEW init fail!" << endl;
        return 1;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 2D
    // createTitle();
    // createMap();
    // createPlanes();
    // createIndicators();
    // createProgressBar();
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 3D

    Shader basic3dShader("basic_3d.vert", "basic_3d.frag");
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 8.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    Model map("res/map.obj");

    //Render petlja
    basic3dShader.use();
    basic3dShader.setVec3("uLightPos", 0, 1, 3);
    basic3dShader.setVec3("uViewPos", 0, 0, 5);
    basic3dShader.setVec3("uLightColor", 1, 1, 1);
    basic3dShader.setMat4("uP", projection);
    basic3dShader.setMat4("uV", view);
    glm::mat4 model = glm::mat4(1.0f);

    glEnable(GL_DEPTH_TEST);
    

    // loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        basic3dShader.use();
        basic3dShader.setMat4("uM", glm::mat4(1.0f));
        map.Draw(basic3dShader);


        // drawTitle();
        // drawMap();
        // drawPlanes(window);
        // drawIndicators(window);
        // drawProgressBars(window);

        // glBindBuffer(GL_ARRAY_BUFFER, 0);
        // glBindVertexArray(0);
        // glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // destroyTitle();
    // destroyMap();
    // destroyPlanes();
    glfwTerminate();
    return 0;
}