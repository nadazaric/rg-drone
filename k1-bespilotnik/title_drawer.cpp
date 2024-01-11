#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "title_drawer.h"
#include "constants.h"
#include "texture_helper.h"
#include "shader_helper.h"

unsigned titleTexture;
unsigned int titleShader;
unsigned int VAO_TITLE, VBO_TITLE;

void createTitle() {
    titleShader = createShader("map.vert", "map.frag");

    float vertices[] =
    {
       TITLE_LEFT, TITLE_BOTTOM,   0.0, 0.0,
       TITLE_LEFT, TITLE_TOP,      0.0, 1.0,
       TITLE_RIGHT, TITLE_BOTTOM,  1.0, 0.0,
       TITLE_RIGHT, TITLE_TOP,     1.0, 1.0
    };

    glGenVertexArrays(1, &VAO_TITLE);
    glBindVertexArray(VAO_TITLE);

    glGenBuffers(1, &VBO_TITLE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TITLE);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (2 + 2) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Texture
    //titleTexture = loadImageToTexture(TITLE_IMAGE_PATH);
    glBindTexture(GL_TEXTURE_2D, titleTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc = glGetUniformLocation(titleShader, "uTex");
    glUniform1i(uTexLoc, 0);

}

void drawTitle() {
    glUseProgram(titleShader);
    glBindVertexArray(VAO_TITLE);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, titleTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void destroyTitle() {
    glDeleteTextures(1, &titleTexture);
    glDeleteBuffers(1, &VBO_TITLE);
    glDeleteVertexArrays(1, &VAO_TITLE);
    glDeleteProgram(titleShader);
}