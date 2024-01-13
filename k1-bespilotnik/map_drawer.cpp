#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "constants.h"
#include "map_drawer.h"
#include "shader_helper.h"
#include "texture_helper.h"

unsigned mapTexture;
unsigned int mapShader, glassShader;
unsigned int VAO_MAP, VBO_MAP;

void createMap() {
    mapShader = createShader("map.vert", "map.frag");
    glassShader = createShader("glass.vert", "glass.frag");

    //Vertices
    float vertices[] =
    {
       MAP_LEFT, MAP_BOTTOM,   0.0, 0.0,    0.1, 0.9, 0.5, 0.3,
       MAP_LEFT, MAP_TOP,      0.0, 1.0,    0.1, 0.9, 0.5, 0.3,
       MAP_RIGHT, MAP_BOTTOM,  1.0, 0.0,    0.1, 0.9, 0.5, 0.3,
       MAP_RIGHT, MAP_TOP,     1.0, 1.0,    0.1, 0.9, 0.5, 0.3,
    };

    unsigned int stride = (2 + 2 + 4) * sizeof(float);
    glGenVertexArrays(1, &VAO_MAP);
    glBindVertexArray(VAO_MAP);

    glGenBuffers(1, &VBO_MAP);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_MAP);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // texture position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // rgba
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Texture
    //mapTexture = loadImageToTexture(MAP_IMAGE_PATH);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc = glGetUniformLocation(mapShader, "uTex");
    glUniform1i(uTexLoc, 0);

};

void drawMap() {
    glBindVertexArray(VAO_MAP);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // draw map
    glUseProgram(mapShader);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    //draw glass
    glUseProgram(glassShader);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void destroyMap() {
    glDeleteTextures(1, &mapTexture);
    glDeleteBuffers(1, &VBO_MAP);
    glDeleteVertexArrays(1, &VAO_MAP);
    glDeleteProgram(mapShader);
}