#include "circle_helper.h";
#include <corecrt_math.h>

float* generateCircle(float x, float y, float radius, float* vertices, int resolution) {
    vertices[0] = x;
    vertices[1] = y;
    for (int i = 0; i <= resolution; i++) {
        vertices[2 + 2 * i] = radius * cos((3.141592 / 180) * (i * 360 / resolution)) + x;
        vertices[2 + 2 * i + 1] = radius * sin((3.141592 / 180) * (i * 360 / resolution)) + y;
    }
    return vertices;
}