#include "planes_drawer.h"
#include "constants.h"
#include "shader_helper.h"
#include "circle_helper.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>

unsigned VAO_PLANE[2], VBO_PLANE[2], VAO_INDICATORS[2], VBO_INDICATORS[2], VAO_PROGRESS_BAR[2], VBO_PROGRESS_BAR[2];
unsigned int planeShader, indicatorShader, progressBarShader, basicShader;

float firstPlaneVerteces[2 * CIRCLE_RESOLUTION + 4], secondPlaneVerteces[2 * CIRCLE_RESOLUTION + 4];
float firstPlaneX, firstPlaneY, secondPlaneX, secondPlaneY;
bool isFirstPlaneDestroyed, isSecondPlaneDestroyed;
unsigned int uLocationFirstPlane, uLocationSecondPlane, uColorFirstPlane, uColorSecondPlane;

unsigned int uFirstIndicatorColor, uSecondIndicatorColor;
bool isFirstPlaneActive, isSecondPlaneActive;

unsigned int uFirstProgress, uFirstProggressStartPos, uFirstProgressMaxWidth, uSecondProgress, uSecondProggressStartPos, uSecondProgressMaxWidth;
float firstProgress = 1.0, secondProgress = 1.0;

void destroyFirstPlane() {
    isFirstPlaneDestroyed = true;
    isFirstPlaneActive = false;
}

void destroySecondPlane() {
    isSecondPlaneDestroyed = true;
    isSecondPlaneActive = false;
}

bool isInRestricted(float x, float y) {
    double distance =
        std::sqrt(std::pow(x - CIRCLE_RESTRICTED_ZONE_CENTER_X, 2) + std::pow(y - CIRCLE_RESTRICTED_ZONE_CENTER_Y, 2));
    return distance - CIRCLE_RESTRICTED_ZONE_RADIUS < 1e-6; // < CIRCLE_PLANE_RADIUS //ako hocu da pri dodiru vec nestane avion
}

bool isColision() {
    if (!isFirstPlaneActive || !isSecondPlaneActive) return false; //ako je jedan prizemljen moze letjeti druga iznad nje
    double distance = std::sqrt(
        std::pow((firstPlaneX + FIRST_AIRPLANE_INITIAL_X) - (secondPlaneX + SECOND_AIRPLANE_INITIAL_X), 2) +
        std::pow((firstPlaneY + FIRST_AIRPLANE_INITIAL_Y) - (secondPlaneY + SECOND_AIRPLANE_INITIAL_Y), 2));
    bool isColision = distance < 2 * CIRCLE_PLANE_RADIUS;
    if (isColision) {
        destroyFirstPlane();
        destroySecondPlane();
    }
    return isColision;
}

void createPlanes() {

    planeShader = createShader("plane.vert", "plane.frag");

    generateCircle(FIRST_AIRPLANE_INITIAL_X, FIRST_AIRPLANE_INITIAL_Y, CIRCLE_PLANE_RADIUS, firstPlaneVerteces, CIRCLE_RESOLUTION);
    generateCircle(SECOND_AIRPLANE_INITIAL_X, SECOND_AIRPLANE_INITIAL_Y, CIRCLE_PLANE_RADIUS, secondPlaneVerteces, CIRCLE_RESOLUTION);

    // first plane
    glGenVertexArrays(1, &VAO_PLANE[0]);
    glGenBuffers(1, &VBO_PLANE[0]);

    glBindVertexArray(VAO_PLANE[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PLANE[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstPlaneVerteces), firstPlaneVerteces, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // secon plane
    glGenVertexArrays(1, &VAO_PLANE[1]);
    glGenBuffers(1, &VBO_PLANE[1]);

    glBindVertexArray(VAO_PLANE[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PLANE[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondPlaneVerteces), secondPlaneVerteces, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    uLocationFirstPlane = glGetUniformLocation(planeShader, "uPos");
    uLocationSecondPlane = glGetUniformLocation(planeShader, "uPos");
    uColorFirstPlane = glGetUniformLocation(planeShader, "uColor");
    uColorSecondPlane = glGetUniformLocation(planeShader, "uColor");
}

void createIndicators() {
    indicatorShader = createShader("indicator.vert", "indicator.frag");

    float firstIndicatorVertices[] =
    {
       FIRST_INDICATOR_LEFT, FIRST_INDICATOR_BOTTOM,
       FIRST_INDICATOR_LEFT, FIRST_INDICATOR_TOP,
       FIRST_INDICATOR_RIGHT, FIRST_INDICATOR_BOTTOM,
       FIRST_INDICATOR_RIGHT, FIRST_INDICATOR_TOP,
    };

    glGenVertexArrays(1, &VAO_INDICATORS[0]);
    glBindVertexArray(VAO_INDICATORS[0]);

    glGenBuffers(1, &VBO_INDICATORS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_INDICATORS[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstIndicatorVertices), firstIndicatorVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    uFirstIndicatorColor = glGetUniformLocation(indicatorShader, "uColor");

    float secondIndicatorVertices[] =
    {
       SECOND_INDICATOR_LEFT, SECOND_INDICATOR_BOTTOM,
       SECOND_INDICATOR_LEFT, SECOND_INDICATOR_TOP,
       SECOND_INDICATOR_RIGHT, SECOND_INDICATOR_BOTTOM,
       SECOND_INDICATOR_RIGHT, SECOND_INDICATOR_TOP,
    };

    glGenVertexArrays(1, &VAO_INDICATORS[1]);
    glBindVertexArray(VAO_INDICATORS[1]);

    glGenBuffers(1, &VBO_INDICATORS[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_INDICATORS[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondIndicatorVertices), secondIndicatorVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    uSecondIndicatorColor = glGetUniformLocation(indicatorShader, "uColor");
}

void createProgressBar() {
    progressBarShader = createShader("progress_bar.vert", "progress_bar.frag");
    basicShader = createShader("basic.vert", "basic.frag");

    float firstProgressBarVertices[] = {
       FIRST_PROGRESS_BAR_LEFT, PROGRESS_BAR_BOTTOM,                          FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3,
       FIRST_PROGRESS_BAR_LEFT, PROGRESS_BAR_TOP,                             FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3,
       FIRST_PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_BOTTOM,     FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3,
       FIRST_PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_TOP,        FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3
    };

    glGenVertexArrays(1, &VAO_PROGRESS_BAR[0]);
    glBindVertexArray(VAO_PROGRESS_BAR[0]);

    glGenBuffers(1, &VBO_PROGRESS_BAR[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PROGRESS_BAR[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstProgressBarVertices), firstProgressBarVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 4) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 4) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    uFirstProgress = glGetUniformLocation(progressBarShader, "uProgress");
    uFirstProggressStartPos = glGetUniformLocation(progressBarShader, "uStartPos");
    uFirstProgressMaxWidth = glGetUniformLocation(progressBarShader, "uMaxWidth");

    float secondProgressBarVertices[] = {
           SECOND_PROGRESS_BAR_LEFT, PROGRESS_BAR_BOTTOM,                      SECOND_PLANE_R, SECOND_PLANE_G, SECOND_PLANE_B, 0.3,
           SECOND_PROGRESS_BAR_LEFT, PROGRESS_BAR_TOP,                         SECOND_PLANE_R, SECOND_PLANE_G, SECOND_PLANE_B, 0.3,
           SECOND_PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_BOTTOM, SECOND_PLANE_R, SECOND_PLANE_G, SECOND_PLANE_B, 0.3,
           SECOND_PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_TOP,    SECOND_PLANE_R, SECOND_PLANE_G, SECOND_PLANE_B, 0.3
    };

    glGenVertexArrays(1, &VAO_PROGRESS_BAR[1]);
    glBindVertexArray(VAO_PROGRESS_BAR[1]);

    glGenBuffers(1, &VBO_PROGRESS_BAR[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_PROGRESS_BAR[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondProgressBarVertices), secondProgressBarVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2 + 4) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (2 + 4) * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    uSecondProgress = glGetUniformLocation(progressBarShader, "uProgress");
    uSecondProggressStartPos = glGetUniformLocation(progressBarShader, "uStartPos");
    uSecondProgressMaxWidth = glGetUniformLocation(progressBarShader, "uMaxWidth");

}

void firstPlaneMoveCommands(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if ((firstPlaneY + FIRST_AIRPLANE_INITIAL_Y > MAP_TOP) || isInRestricted(firstPlaneX + FIRST_AIRPLANE_INITIAL_X, firstPlaneY + FIRST_AIRPLANE_INITIAL_Y) || isColision())
            destroyFirstPlane();
        else firstPlaneY += PLANE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if ((firstPlaneY + FIRST_AIRPLANE_INITIAL_Y < MAP_BOTTOM) || isInRestricted(firstPlaneX + FIRST_AIRPLANE_INITIAL_X, firstPlaneY + FIRST_AIRPLANE_INITIAL_Y) || isColision())
            destroyFirstPlane();
        else firstPlaneY -= PLANE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        if ((firstPlaneX + FIRST_AIRPLANE_INITIAL_X > MAP_RIGHT) || isInRestricted(firstPlaneX + FIRST_AIRPLANE_INITIAL_X, firstPlaneY + FIRST_AIRPLANE_INITIAL_Y) || isColision())
            destroyFirstPlane();
        else firstPlaneX += PLANE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        if ((firstPlaneX + FIRST_AIRPLANE_INITIAL_X < MAP_LEFT) || isInRestricted(firstPlaneX + FIRST_AIRPLANE_INITIAL_X, firstPlaneY + FIRST_AIRPLANE_INITIAL_Y) || isColision())
            destroyFirstPlane();
        else firstPlaneX -= PLANE_SPEED;
    }
}

void firstPlaneActivationCommand(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        if (isColision()) return; // provjeri da li se pri polijetanju drugi avion nalazi iznad, ako jeste ne postavljaj avion da je aktivan
        isFirstPlaneActive = true;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) isFirstPlaneActive = false;
}

void secondPlaneMoveCommands(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if ((secondPlaneY + SECOND_AIRPLANE_INITIAL_Y > MAP_TOP) || isInRestricted(secondPlaneX + SECOND_AIRPLANE_INITIAL_X, secondPlaneY + SECOND_AIRPLANE_INITIAL_Y) || isColision())
            destroySecondPlane();
        else secondPlaneY += PLANE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if ((secondPlaneY + SECOND_AIRPLANE_INITIAL_Y < MAP_BOTTOM) || isInRestricted(secondPlaneX + SECOND_AIRPLANE_INITIAL_X, secondPlaneY + SECOND_AIRPLANE_INITIAL_Y) || isColision())
            destroySecondPlane();
        else secondPlaneY -= PLANE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        if ((secondPlaneX + SECOND_AIRPLANE_INITIAL_X > MAP_RIGHT) || isInRestricted(secondPlaneX + SECOND_AIRPLANE_INITIAL_X, secondPlaneY + SECOND_AIRPLANE_INITIAL_Y) || isColision())
            destroySecondPlane();
        else secondPlaneX += PLANE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if ((secondPlaneX + SECOND_AIRPLANE_INITIAL_X < MAP_LEFT) || isInRestricted(secondPlaneX + SECOND_AIRPLANE_INITIAL_X, secondPlaneY + SECOND_AIRPLANE_INITIAL_Y) || isColision())
            destroySecondPlane();
        else secondPlaneX -= PLANE_SPEED;
    }
}

void secondPlaneActivationCommand(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
    {
        if (isColision()) return;
        isSecondPlaneActive = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) isSecondPlaneActive = false;
}

void drawPlanes(GLFWwindow* window) {
    if (!isFirstPlaneDestroyed && isFirstPlaneActive) firstPlaneMoveCommands(window);
    if (!isSecondPlaneDestroyed && isSecondPlaneActive) secondPlaneMoveCommands(window);

    glUseProgram(planeShader);

    // first plane
    glBindVertexArray(VAO_PLANE[0]);
    glUniform2f(uLocationFirstPlane, firstPlaneX, firstPlaneY);
    glUniform3f(uColorFirstPlane, FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B);
    if (!isFirstPlaneDestroyed) glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(firstPlaneVerteces) / (2 * sizeof(float)));
    // second plane
    glBindVertexArray(VAO_PLANE[1]);
    glUniform2f(uLocationSecondPlane, secondPlaneX, secondPlaneY);
    glUniform3f(uColorFirstPlane, SECOND_PLANE_R, SECOND_PLANE_G, SECOND_PLANE_B);
    if (!isSecondPlaneDestroyed) glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(secondPlaneVerteces) / (2 * sizeof(float)));
}

void drawIndicators(GLFWwindow* window) {

    if (!isFirstPlaneDestroyed) firstPlaneActivationCommand(window);
    if (!isSecondPlaneDestroyed) secondPlaneActivationCommand(window);

    glUseProgram(indicatorShader);

    // first indicator
    if (isFirstPlaneActive) glUniform4f(uFirstIndicatorColor, FIRST_INDICATOR_R, FIRST_INDICATOR_G, FIRST_INDICATOR_B, 1.0);
    else glUniform4f(uFirstIndicatorColor, INACTIV_INDICATOR_R, INACTIV_INDICATOR_G, INACTIV_INDICATOR_B, 1.0);
    glBindVertexArray(VAO_INDICATORS[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // second indicator
    if (isSecondPlaneActive) glUniform4f(uSecondIndicatorColor, SECOND_INDICATOR_R, SECOND_INDICATOR_G, SECOND_INDICATOR_B, 1.0);
    else glUniform4f(uSecondIndicatorColor, INACTIV_INDICATOR_R, INACTIV_INDICATOR_G, INACTIV_INDICATOR_B, 1.0);
    glBindVertexArray(VAO_INDICATORS[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void drawProgressBars(GLFWwindow* window) {

    if (isFirstPlaneActive && firstProgress > 0)
    {
        firstProgress -= PROGRESS_BAR_OFFSET;
        if (firstProgress <= 0) destroyFirstPlane();
    }
    if (isSecondPlaneActive && secondProgress > 0)
    {
        secondProgress -= PROGRESS_BAR_OFFSET;
        if (secondProgress <= 0) destroySecondPlane();
    }

    glUseProgram(progressBarShader);

    // first progress bar
    glBindVertexArray(VAO_PROGRESS_BAR[0]);
    glUniform1f(uFirstProgress, firstProgress);
    glUniform1f(uFirstProggressStartPos, FIRST_PROGRESS_BAR_LEFT);
    glUniform1f(uFirstProgressMaxWidth, PROGRESS_BAR_WIDTH);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // second progress bar
    glBindVertexArray(VAO_PROGRESS_BAR[1]);
    glUniform1f(uSecondProgress, secondProgress);
    glUniform1f(uSecondProggressStartPos, SECOND_PROGRESS_BAR_LEFT);
    glUniform1f(uSecondProgressMaxWidth, PROGRESS_BAR_WIDTH);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(basicShader);
    glBindVertexArray(VAO_PROGRESS_BAR[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(VAO_PROGRESS_BAR[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void destroyPlanes() {
    glDeleteBuffers(2, VBO_PLANE);
    glDeleteVertexArrays(2, VAO_PLANE);
    glDeleteProgram(planeShader);
    glDeleteBuffers(2, VBO_INDICATORS);
    glDeleteVertexArrays(2, VBO_INDICATORS);
    glDeleteProgram(indicatorShader);
    glDeleteBuffers(2, VBO_PROGRESS_BAR);
    glDeleteVertexArrays(2, VAO_PROGRESS_BAR);
    glDeleteProgram(progressBarShader);
}