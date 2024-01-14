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
#include "circle_helper.h"
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

void topViewport() {
    glViewport(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT / 2); 
}

void bottomViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 2);    
}

void mapViewport(){
    glViewport(WINDOW_WIDTH - MAP_WIDTH, 0, MAP_WIDTH, MAP_HEIGHT);
}

void fullViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void draw2D() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
}

void draw3D() {
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

    unsigned int VAO[7];
    glGenVertexArrays(7, VAO);
    unsigned int VBO[7];
    glGenBuffers(7, VBO);

    // Shaders
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");
    unsigned int airplaneShader = createShader("plane.vert", "plane.frag");
    unsigned int progressBarShader = createShader("progress_bar.vert", "progress_bar.frag");

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
        MAP_LEFT, MAP_BOTTOM,   0.0, 0.0,    
        MAP_LEFT, MAP_TOP,      0.0, 1.0,    
        MAP_RIGHT, MAP_BOTTOM,  1.0, 0.0,    
        MAP_RIGHT, MAP_TOP,     1.0, 1.0,   
     };
    unsigned int mapStride = (2 + 2) * sizeof(float);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMap), verticesMap, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, mapStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, mapStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned mapTexture = loadImageToTexture(MAP_IMAGE_PATH);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Glass (from verticesMap)
    unsigned int uColorGlass = glGetUniformLocation(basicShader, "uColor");

    // Restricted Zone
    float verticesRestrictedZone[CIRCLE_RESOLUTION * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    generateCircle(CIRCLE_RESTRICTED_ZONE_CENTER_X, CIRCLE_RESTRICTED_ZONE_CENTER_Y, CIRCLE_RESTRICTED_ZONE_RADIUS, verticesRestrictedZone, CIRCLE_RESOLUTION);
    unsigned int circleStride = 2 * sizeof(float);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRestrictedZone), verticesRestrictedZone, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, circleStride, (void*)0);
    glEnableVertexAttribArray(0);
    
    unsigned int uColorRestrictedArea = glGetUniformLocation(basicShader, "uColor");

    // Airplanes
    float verticesFirstAirplane[CIRCLE_RESOLUTION * 2 + 4];
    generateCircle(FIRST_AIRPLANE_INITIAL_X, FIRST_AIRPLANE_INITIAL_Y, CIRCLE_PLANE_RADIUS, verticesFirstAirplane, CIRCLE_RESOLUTION);

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFirstAirplane), verticesFirstAirplane, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, circleStride, (void*)0);
    glEnableVertexAttribArray(0);
    
    float verticesSecondAirplane[CIRCLE_RESOLUTION * 2 + 4];
    generateCircle(SECOND_AIRPLANE_INITIAL_X, SECOND_AIRPLANE_INITIAL_Y, CIRCLE_PLANE_RADIUS, verticesSecondAirplane, CIRCLE_RESOLUTION);

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSecondAirplane), verticesSecondAirplane, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, circleStride, (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int uPositionFirstAirplane = glGetUniformLocation(airplaneShader, "uPos");
    unsigned int uPositionSecondAirplane = glGetUniformLocation(airplaneShader, "uPos");

    // Indicators
    bool isFirstAirplaneActive = false;
    bool isSecondAirplaneActive = false;
    bool isFirstAirplaneDestroyed = false;
    bool isSecondAirplaneDestroyed = false;
    float verticesIndicators[] =
    {
        INDICATOR_LEFT, FIRST_INDICATOR_BOTTOM,
        INDICATOR_LEFT, INDICATOR_TOP,
        INDICATOR_RIGHT, FIRST_INDICATOR_BOTTOM,
        INDICATOR_RIGHT, INDICATOR_TOP,
    };
    unsigned int indicatorStride = 2 * sizeof(float);

    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesIndicators), verticesIndicators, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, indicatorStride, (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int uColorIndicator = glGetUniformLocation(basicShader, "uColor");

    // Progress Bars
    float firstAirplaneProgress = 1.0f;
    float secondAirplaneProgress = 1.0f;
    float verticesProgressBar[] = {
        PROGRESS_BAR_LEFT, PROGRESS_BAR_BOTTOM,                          FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3,
        PROGRESS_BAR_LEFT, PROGRESS_BAR_TOP,                             FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3,
        PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_BOTTOM,     FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3,
        PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_TOP,        FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.3
    };
    unsigned int progressBarStride = (2 + 4) * sizeof(float);

    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesProgressBar), verticesProgressBar, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, progressBarStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, progressBarStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int uSecondAirplaneProgress = glGetUniformLocation(progressBarShader, "uProgress");
    unsigned int uSecondAirplaneProgressStartPos = glGetUniformLocation(progressBarShader, "uStartPos");
    unsigned int uSecondAirplaneProgressMaxWidth = glGetUniformLocation(progressBarShader, "uMaxWidth");
    
    // createMap();
    // createPlanes();
    // createIndicators();
    // createProgressBar();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 3D

    Shader basic3dShader("basic_3d.vert", "basic_3d.frag");
    glm::mat4 projection = glm::perspective(glm::radians(50.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    glm::mat4 firstCameraView;
    glm::vec3 firstCameraPosition = glm::vec3(FIRST_AIRPLANE_INITIAL_X, FIRST_AIRPLANE_INITIAL_HEIGHT, FIRST_AIRPLANE_INITIAL_Y + 1.0f);
    glm::vec3 firstCameraFront = glm::vec3(0.0f, 0.0f, 0.1f); // vektor koji odredjuje smijer gledanja kamere
    glm::vec3 firstCameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // znaci da je "gore" usmjereno prema pozitivnom y-smjeru (oznacava sta je gore u odnosu na kameru)
    float firstCameraYaw = -90.0f; // vrijednost "skretanja" koliko idemo gledamo ulijevo ili udesno po horizontali (yaw value which represents the magnitude we're looking to the left or to the right)
    float firstCameraPitch = 0.0f; // koliko gledamo gore/dole (angle that depicts how much we're looking up or down)

    glm::mat4 secondCameraView;
    glm::vec3 secondCameraPosition = glm::vec3(SECOND_AIRPLANE_INITIAL_X, SECOND_AIRPLANE_INITIAL_HEIGHT, SECOND_AIRPLANE_INITIAL_Y + 1.0f);
    glm::vec3 secondCameraFront = glm::vec3(0.0f, 0.0f, 0.1f);
    glm::vec3 secondCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float secondCameraYaw = -90.0f;
    float secondCameraPitch = 0.0f;
    
    Model map("res/map.obj");

    // Postavi svjetlo i projekciju
    basic3dShader.use();
    basic3dShader.setVec3("uLightPos", 0, 1, 3);
    basic3dShader.setVec3("uViewPos", 0, 0, 5);
    basic3dShader.setVec3("uLightColor", 1, 1, 1);
    basic3dShader.setMat4("uP", projection);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.84f, 0.93f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Keys Events
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (isFirstAirplaneActive)
        {
            if (firstAirplaneProgress >= 0) firstAirplaneProgress -= PROGRESS_BAR_OFFSET;
            else {
                isFirstAirplaneActive = false;
                isFirstAirplaneDestroyed = true;
            }
            firstAirplaneProgress -= PROGRESS_BAR_OFFSET;
            // Prva kamera
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                firstCameraPosition += CAMERA_SPEED * glm::vec3(firstCameraFront.x, 0.0f, firstCameraFront.z);
            // pomjeranje unaprijed, odnosno u smijeru u koji kamera gleda, firstCameraFront je vektor koji označava smjer gledanja kamere,
            // i njega mnozim sa brzinom kretanja, pa to dodam na trenutnu poziciju kamere
            // NAPOMENA:  da bih zadrzala zeljeni pravac, moram da uzmem x i z koordinate fronta, ali z moram da vratim na 0 da
            // ne bi moja kamera pocela da se krece prema zemlji i od zemlje umjesto naprijed i nazad
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                firstCameraPosition -= CAMERA_SPEED * glm::vec3(firstCameraFront.x, 0.0f, firstCameraFront.z);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                firstCameraPosition -= glm::normalize(glm::cross(firstCameraFront, firstCameraUp)) * CAMERA_SPEED;
            // pomjeranje ulijeo, glm::cross(firstCameraFront, firstCameraUp)` daje vektor normalan na povrsinu
            // cross - daje vektorski proizvod, taj proizvod je ustvari normala na povrsinu
            // tu normalu normalizujemo (0-1) i onda taj vektor mnozimo sa brzinom kretanja
            // onda to sve oduzmemo/ dodamo u zavisnosti od kretanja lijevo/desno od trenutne pozicije
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                firstCameraPosition += glm::normalize(glm::cross(firstCameraFront, firstCameraUp)) * CAMERA_SPEED; 
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
                if (firstCameraPosition.y <= MAX_HEIGHT) firstCameraPosition  += CAMERA_SPEED * firstCameraUp;
            // posto cameraUp oznacava sta je inad kamera (u ovom slucaju penjemo se po y osi, onda cameraUp ima (0,1,0) vrijednosti
            // onda to mnozimo s brzinom i dodajemo na poziciju kamere
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                firstCameraPosition -= CAMERA_SPEED * firstCameraUp;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                firstCameraYaw -= CAMERA_SENSITIVITY;
            // posto yaw oznacava koliko idemo lijevo i desno na trenutnoj ravni, horizontalno, onda samo dodam/oduzmem ako hocu da vrsim rotaciju kamere
            // yaw potreban kasnije u kodu za izracunavanje novog cameraFront
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                firstCameraYaw += CAMERA_SENSITIVITY;
                
        }

        if (isSecondAirplaneActive)
        {
            if (secondAirplaneProgress >= 0) {
                secondAirplaneProgress -= PROGRESS_BAR_OFFSET;
            }
            else {
                isSecondAirplaneActive = false;
                isSecondAirplaneDestroyed = true;
            }
            
            // Druga kamera
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                secondCameraPosition += CAMERA_SPEED * glm::vec3(secondCameraFront.x, 0.0f, secondCameraFront.z);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                secondCameraPosition -= CAMERA_SPEED * glm::vec3(secondCameraFront.x, 0.0f, secondCameraFront.z);
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                secondCameraPosition -= glm::normalize(glm::cross(secondCameraFront, secondCameraUp)) * CAMERA_SPEED;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                secondCameraPosition += glm::normalize(glm::cross(secondCameraFront, secondCameraUp)) * CAMERA_SPEED;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                if (secondCameraPosition.y <= MAX_HEIGHT) secondCameraPosition += CAMERA_SPEED * secondCameraUp;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                secondCameraPosition -= CAMERA_SPEED * secondCameraUp;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                secondCameraYaw -= CAMERA_SENSITIVITY;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                secondCameraYaw += CAMERA_SENSITIVITY;
            }
                
        }
        
        // On/Off
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !isFirstAirplaneDestroyed) isFirstAirplaneActive = true;
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) isFirstAirplaneActive = false;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !isSecondAirplaneDestroyed) isSecondAirplaneActive = true;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) isSecondAirplaneActive = false;
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 3D Render
        draw3D();

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Top Window
        topViewport();
        basic3dShader.use();
        
        firstCameraPitch = glm::clamp(firstCameraPitch, -90.0f, -CAMERA_ANGLE); // Ograničavanje nagiba kamere
        glm::vec3 front; // Računanje novog vektora gledišta
        front.x = cos(glm::radians(firstCameraYaw)) * cos(glm::radians(firstCameraPitch)); // yaw koliko smo lijevo/desno stepeni gledajuci po horizontali (koliko smo rotirani)
        front.y = sin(glm::radians(firstCameraPitch)); // nagib kamere
        front.z = sin(glm::radians(firstCameraYaw)) * cos(glm::radians(firstCameraPitch));
        firstCameraFront = normalize(front);
        
        firstCameraView = lookAt(firstCameraPosition, firstCameraPosition + firstCameraFront, firstCameraUp);
        basic3dShader.setMat4("uV", firstCameraView);
        
        basic3dShader.setMat4("uM", glm::mat4(1.0f)); // Prikaz mape za prvu kameru
        map.Draw(basic3dShader);

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Bottom Window
        bottomViewport();
        
        // Računanje novog vektora gledišta
        secondCameraPitch = glm::clamp(secondCameraPitch, -90.0f, -CAMERA_ANGLE);
        front.x = cos(glm::radians(secondCameraYaw)) * cos(glm::radians(secondCameraPitch));
        front.y = sin(glm::radians(secondCameraPitch));
        front.z = sin(glm::radians(secondCameraYaw)) * cos(glm::radians(secondCameraPitch));
        secondCameraFront = glm::normalize(front);
        
        // Postavljanje kamere na zeljenu poziciju
        secondCameraView = glm::lookAt(secondCameraPosition, secondCameraPosition + secondCameraFront, secondCameraUp);
        basic3dShader.setMat4("uV", secondCameraView);
        
        basic3dShader.setMat4("uM", glm::mat4(1.0f));
        map.Draw(basic3dShader);

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 2D Render
        draw2D();
        
        // Name
        fullViewport();
        glUseProgram(textureShader);
        glBindVertexArray(VAO[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, nameTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Map
        mapViewport();
        glBindVertexArray(VAO[1]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mapTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // already active texture shader
        glUseProgram(basicShader);
        glUniform4f(uColorGlass, 0.6, 0.9, 0.5, 0.25);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Restricted Zone
        glBindVertexArray(VAO[2]);
        glUniform4f(uColorRestrictedArea, 1.0f, 0.0f, 0.0f, 0.2f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(verticesRestrictedZone) / (2 * sizeof(float)));

        // Airplanes
        glUseProgram(airplaneShader);
        glBindVertexArray(VAO[3]);
        glUniform2f(uPositionFirstAirplane, firstCameraPosition.x - FIRST_AIRPLANE_INITIAL_X, - firstCameraPosition.z - FIRST_AIRPLANE_INITIAL_Y);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(verticesFirstAirplane) / (2 * sizeof(float)));
        
        glBindVertexArray(VAO[4]);
        glUniform2f(uPositionSecondAirplane, secondCameraPosition.x - SECOND_AIRPLANE_INITIAL_X, - secondCameraPosition.z - SECOND_AIRPLANE_INITIAL_Y);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(verticesSecondAirplane) / (2 * sizeof(float)));

        // Indicators
        topViewport();
        glUseProgram(basicShader);
        glBindVertexArray(VAO[5]);
        if (isFirstAirplaneActive) glUniform4f(uColorIndicator, INDICATOR_R, INDICATOR_G, INDICATOR_B, 1.0);
        else glUniform4f(uColorIndicator, INACTIVE_INDICATOR_R, INACTIVE_INDICATOR_G, INACTIVE_INDICATOR_B, 1.0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(VAO[6]);
        glUniform4f(uColorRestrictedArea, FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.4f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(progressBarShader);
        glUniform1f(uSecondAirplaneProgress, firstAirplaneProgress);
        glUniform1f(uSecondAirplaneProgressStartPos, PROGRESS_BAR_LEFT);
        glUniform1f(uSecondAirplaneProgressMaxWidth, PROGRESS_BAR_WIDTH);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        
        bottomViewport();
        glUseProgram(basicShader);
        glBindVertexArray(VAO[5]);
        if (isSecondAirplaneActive) glUniform4f(uColorIndicator, INDICATOR_R, INDICATOR_G, INDICATOR_B, 1.0);
        else glUniform4f(uColorIndicator, INACTIVE_INDICATOR_R, INACTIVE_INDICATOR_G, INACTIVE_INDICATOR_B, 1.0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(VAO[6]);
        glUniform4f(uColorRestrictedArea, FIRST_PLANE_R, FIRST_PLANE_G, FIRST_PLANE_B, 0.4f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(progressBarShader);
        glUniform1f(uSecondAirplaneProgress, secondAirplaneProgress);
        glUniform1f(uSecondAirplaneProgressStartPos, PROGRESS_BAR_LEFT);
        glUniform1f(uSecondAirplaneProgressMaxWidth, PROGRESS_BAR_WIDTH);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        // drawMap();
        // drawPlanes(window); 
        // drawProgressBars(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // destroyMap();
    // destroyPlanes();

    // Delete Textures
    glDeleteTextures(1, &nameTexture);
    glDeleteTextures(1, &mapTexture);

    // Delete VBO and VAO
    glDeleteBuffers(7, VBO);
    glDeleteVertexArrays(7, VAO);

    // Delete shaders
    glDeleteProgram(basicShader);
    glDeleteProgram(textureShader);
    
    glfwTerminate();
    return 0;
}