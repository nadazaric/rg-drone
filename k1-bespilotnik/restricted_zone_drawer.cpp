#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <corecrt_math.h>
#include "restricted_zone_drawer.h"
#include "shader_helper.h"
#include "constants.h"
#include "circle_helper.h"
using namespace std;

unsigned int restrictedZoneShader;
unsigned int VAO_RESTRICTED, VBO_RESTRICTED;
float circle_vertices[CIRCLE_RESOLUTION * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla

void createRestrictedZone() {
    restrictedZoneShader = createShader("restricted_zone.vert", "restricted_zone.frag");

    generateCircle(CIRCLE_RESTRICTED_ZONE_CENTER_X, CIRCLE_RESTRICTED_ZONE_CENTER_Y, CIRCLE_RESTRICTED_ZONE_RADIUS, circle_vertices, CIRCLE_RESOLUTION);

    glGenVertexArrays(1, &VAO_RESTRICTED);
    glBindVertexArray(VAO_RESTRICTED);
    glGenBuffers(1, &VBO_RESTRICTED);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_RESTRICTED);

    glBindVertexArray(VAO_RESTRICTED);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_RESTRICTED);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_vertices), circle_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glUseProgram(restrictedZoneShader);
    GLint circleColorUnif = glGetUniformLocation(restrictedZoneShader, "circleColor");
    glUniform4f(circleColorUnif, 1.0f, 0.0f, 0.0f, 0.3f);
    glUseProgram(0);
}

void drawRestrictedZone() {
    glUseProgram(restrictedZoneShader);
    glBindVertexArray(VAO_RESTRICTED);
    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle_vertices) / (2 * sizeof(float)));
}
void destroyRestrictedZone() {
    glDeleteBuffers(1, &VBO_RESTRICTED);
    glDeleteVertexArrays(1, &VAO_RESTRICTED);
    glDeleteProgram(restrictedZoneShader);
}