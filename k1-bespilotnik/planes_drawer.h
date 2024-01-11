#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
void createPlanes();
void createIndicators();
void createProgressBar();
void drawPlanes(GLFWwindow* window);
void drawIndicators(GLFWwindow* window);
void drawProgressBars(GLFWwindow* window);
void destroyPlanes();