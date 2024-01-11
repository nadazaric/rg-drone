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
using namespace std;

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL) {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

void draw2D() {
    glViewport(WINDOW_WIDTH - MAP_WIDTH, 0, MAP_WIDTH, MAP_HEIGHT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
}
void drawName2D() {
    //glViewport(0, HEIGHT - NAME_HEIGHT, NAME_WIDTH, NAME_HEIGHT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
}
void draw3D() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_CULL_FACE);
}

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

    unsigned int VAO[4];
    glGenVertexArrays(4, VAO);
    unsigned int VBO[4];
    glGenBuffers(4, VBO);

    // Shaders
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    // Name
    float verticesName[] =
    {
        TITLE_LEFT, TITLE_BOTTOM,   0.0, 0.0,
        TITLE_LEFT, TITLE_TOP,      0.0, 1.0,
        TITLE_RIGHT, TITLE_BOTTOM,  1.0, 0.0,
        TITLE_RIGHT, TITLE_TOP,     1.0, 1.0
    };
    unsigned int nameStride = (2 + 2) * sizeof(float);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesName), verticesName, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, nameStride, ((void*)0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, nameStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned nameTexture = loadImageToTexture(TITLE_IMAGE_PATH);
    glBindTexture(GL_TEXTURE_2D, nameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Map
    float verticesMap[] =
    {
        MAP_LEFT, MAP_BOTTOM,   0.0, 0.0,    0.1, 0.9, 0.5, 0.3,
        MAP_LEFT, MAP_TOP,      0.0, 1.0,    0.1, 0.9, 0.5, 0.3,
        MAP_RIGHT, MAP_BOTTOM,  1.0, 0.0,    0.1, 0.9, 0.5, 0.3,
        MAP_RIGHT, MAP_TOP,     1.0, 1.0,    0.1, 0.9, 0.5, 0.3,
     };
    unsigned int mapStride = (2 + 2 + 4) * sizeof(float);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMap), verticesMap, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, mapStride, (void*)0);
    glEnableVertexAttribArray(0);
    // texture position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, mapStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // rgba (for glass)
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, mapStride, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned mapTexture = loadImageToTexture(MAP_IMAGE_PATH);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    
    // createTitle();
    // createMap();
    // createPlanes();
    // createIndicators();
    // createProgressBar();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    //glEnable(GL_DEPTH_TEST);
    

    // loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // 3D

        // Map
        draw3D();

        basic3dShader.use();
        basic3dShader.setMat4("uM", glm::mat4(1.0f));
        map.Draw(basic3dShader);

        // 2D
        
        // Name
        drawName2D();
        glUseProgram(textureShader);
        glBindVertexArray(VAO[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, nameTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Map
        draw2D();
        glBindVertexArray(VAO[1]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mapTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // already active texture shader
        glUseProgram(basicShader);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //drawTitle();
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

    //destroyTitle();
    // destroyMap();
    // destroyPlanes();

    // Delete Textures
    glDeleteTextures(1, &nameTexture);
    glDeleteTextures(1, &mapTexture);

    // Delete VBO and VAO
    glDeleteBuffers(4, VBO);
    glDeleteVertexArrays(4, VAO);

    // Delete shaders
    glDeleteProgram(basicShader);
    glDeleteProgram(textureShader);
    
    glfwTerminate();
    return 0;
}