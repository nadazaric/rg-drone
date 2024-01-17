#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "shader_helper.h"
#include "constants.h"
#include "model.hpp"
#include "shader.hpp"
#include "circle_helper.h"
using namespace std;

// Drone bools
bool isFirstDroneActive = false;
bool isSecondDroneActive = false;
bool isFirstDroneDestroyed = false;
bool isSecondDroneDestroyed = false;
bool isFirstDroneOnLand = true;
bool isSecondDroneOnLand = true;
bool isFirstDroneCameraActive = false;
bool isSecondDroneCameraActive = false;

// Drone consts
float firstDroneConsumptionProgress = 1.0f;
float secondDroneConsumptionProgress = 1.0f;
float firstDroneConsumption = 0.0f;
float secondDroneConsumption = 0.0f;

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

void moveTo(glm::mat4& mat, float x, float y, float z) {
    mat[3][0] = x;
    mat[3][1] = y - 0.1f;
    mat[3][2] = z;
}

void rotateTo(glm::mat4& mat, float angle) {
    mat = glm::rotate(mat, -glm::radians(angle - INITIAL_CAMERA_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
}

void turnOnDrone(int droneNum)
{
    if (droneNum == 1 && !isFirstDroneActive && firstDroneConsumptionProgress > 0.0f) {
        isFirstDroneActive = true;
        firstDroneConsumption += CONSUMPTION_DRONE_ACTIVE;
    } else if (droneNum == 2 && !isSecondDroneActive && secondDroneConsumptionProgress > 0.0f)  {
        isSecondDroneActive = true;
        secondDroneConsumption += CONSUMPTION_DRONE_ACTIVE;
    }
}

void turnOffDrone(int droneNum)
{
    if (droneNum == 1 && isFirstDroneActive) {
        isFirstDroneActive = false;
        firstDroneConsumption -= CONSUMPTION_DRONE_ACTIVE;
    } else if (droneNum == 2 && isSecondDroneActive) {
        isSecondDroneActive = false;
        secondDroneConsumption -= CONSUMPTION_DRONE_ACTIVE;
    }
}

void turnOnCamera(int droneNum)
{
    if (droneNum == 1 && !isFirstDroneCameraActive && firstDroneConsumptionProgress > 0.0f) {
        isFirstDroneCameraActive = true;
        firstDroneConsumption += CONSUMPTION_CAMERA_ON;
    } else if (droneNum == 2 && !isSecondDroneCameraActive && secondDroneConsumptionProgress > 0.0f)  {
        isSecondDroneCameraActive = true;
        secondDroneConsumption += CONSUMPTION_CAMERA_ON;
    }
}

void turnOffCamera(int droneNum)
{
    if (droneNum == 1 && isFirstDroneCameraActive) {
        isFirstDroneCameraActive = false;
        firstDroneConsumption -= CONSUMPTION_CAMERA_ON;
    } else if (droneNum == 2 && isSecondDroneCameraActive) {
        isSecondDroneCameraActive = false;
        secondDroneConsumption -= CONSUMPTION_CAMERA_ON;
    }
}

void destroyDrone(int droneNum){
    if (droneNum == 1) {
        isFirstDroneDestroyed = true;
        isFirstDroneOnLand = true;
    } else {
        isSecondDroneDestroyed = true;
        isSecondDroneOnLand = true;
    }
    turnOffCamera(droneNum);
    turnOffDrone(droneNum);
}

bool isOutOfMap(glm::vec3 drone) {
    return drone.x > MAP_RIGHT || drone.x < MAP_LEFT || drone.z > MAP_TOP || drone.z < MAP_BOTTOM;
}

bool isInRestricted(glm::vec3 drone) {
    double distance =
        std::sqrt(std::pow(drone.x - CIRCLE_RESTRICTED_ZONE_CENTER_X, 2) + std::pow(-drone.z - CIRCLE_RESTRICTED_ZONE_CENTER_Y, 2));
    return distance - CIRCLE_RESTRICTED_ZONE_RADIUS < 1e-6; // < CIRCLE_PLANE_RADIUS //ako hocu da pri dodiru vec nestane avion
}

void setFront(glm::vec3& cameraFront, float& cameraPitch, float cameraYaw) {
    cameraPitch = glm::clamp(cameraPitch, -90.0f, -CAMERA_ANGLE); // Ograničavanje nagiba kamere
    glm::vec3 front; // Računanje novog vektora gledišta
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)); // yaw koliko smo lijevo/desno stepeni gledajuci po horizontali (koliko smo rotirani)
    front.y = sin(glm::radians(cameraPitch)); // nagib kamere
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = normalize(front);
}

bool isColision(glm::vec3 firstDrone, glm::vec3 secondDrone) {
    if (isFirstDroneDestroyed || isSecondDroneDestroyed) return false;
    
    float firstMinX = firstDrone.x - DRONE_OUTBOX_WIDTH / 2.0f;
    float firstMaxX = firstDrone.x + DRONE_OUTBOX_WIDTH / 2.0f;
    float firstMinY = firstDrone.y - DRONE_OUTBOX_HEIGHT / 2.0f;
    float firstMaxY = firstDrone.y + DRONE_OUTBOX_HEIGHT / 2.0f;
    float firstMinZ = firstDrone.z - DRONE_OUTBOX_LENGTH / 2.0f;
    float firstMaxZ = firstDrone.z + DRONE_OUTBOX_LENGTH / 2.0f;

    float secondMinX = secondDrone.x - DRONE_OUTBOX_WIDTH / 2.0f;
    float secondMaxX = secondDrone.x + DRONE_OUTBOX_WIDTH / 2.0f;
    float secondMinY = secondDrone.y - DRONE_OUTBOX_HEIGHT / 2.0f;
    float secondMaxY = secondDrone.y + DRONE_OUTBOX_HEIGHT / 2.0f;
    float secondMinZ = secondDrone.z - DRONE_OUTBOX_LENGTH / 2.0f;
    float secondMaxZ = secondDrone.z + DRONE_OUTBOX_LENGTH / 2.0f;
    
    bool collisionX = firstMaxX >= secondMinX && firstMinX <= secondMaxX;
    bool collisionY = firstMaxY >= secondMinY && firstMinY <= secondMaxY;
    bool collisionZ = firstMaxZ >= secondMinZ && firstMinZ <= secondMaxZ;
    return collisionX && collisionY && collisionZ;
}

float mapValue(float value, float fromMin, float fromMax, float toMin, float toMax) {
    return (value - fromMin) * (toMax - toMin) / (fromMax - fromMin) + toMin;
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
    glfwSwapInterval(1); // vertikalna sinhronizacija ukljucena - brzina renderovanja jednaka refresh rate monitora

    if (glewInit() != GLEW_OK) {
        cout << "GLEW init fail!" << endl;
        return 1;
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 2D

    unsigned int VAO[8];
    glGenVertexArrays(8, VAO);
    unsigned int VBO[8];
    glGenBuffers(8, VBO);

    // Shaders
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");
    unsigned int droneShader = createShader("drone.vert", "drone.frag");
    unsigned int progressBarShader = createShader("progress_bar.vert", "progress_bar.frag");

    // Uniforms
    unsigned int uBasicShaderColor = glGetUniformLocation(basicShader, "uColor");
    unsigned int uDroneShaderModelMatrix = glGetUniformLocation(droneShader, "uModelMatrix");
    unsigned int uDroneShaderColor = glGetUniformLocation(droneShader, "uColor");
    unsigned int uProgressBarShaderColor = glGetUniformLocation(progressBarShader, "uColor");
    unsigned int uProgressBarShaderProgress = glGetUniformLocation(progressBarShader, "uProgress");
    unsigned int uProgressBarShaderStartPosition = glGetUniformLocation(progressBarShader, "uStartPos");
    unsigned int uProgressBarShaderMaxWidth = glGetUniformLocation(progressBarShader, "uMaxWidth");

    // Set Uniforms
    glUseProgram(droneShader);
    glUniform4f(uDroneShaderColor, DRONE_R, DRONE_G, DRONE_B, 1.0);
    glUseProgram(progressBarShader);
    glUniform3f(uProgressBarShaderColor, DRONE_R, DRONE_G, DRONE_B);
    glUniform1f(uProgressBarShaderStartPosition, PROGRESS_BAR_LEFT);
    glUniform1f(uProgressBarShaderStartPosition, PROGRESS_BAR_LEFT);
    glUniform1f(uProgressBarShaderMaxWidth, PROGRESS_BAR_WIDTH);

    // Name
    float verticesName[] =
    {
        //   X          Y          U    V
        NAME_LEFT, NAME_BOTTOM,   0.0, 0.0,
        NAME_LEFT, NAME_TOP,      0.0, 1.0,
        NAME_RIGHT, NAME_BOTTOM,  1.0, 0.0,
        NAME_RIGHT, NAME_TOP,     1.0, 1.0
    };
    unsigned int nameStride = (2 + 2) * sizeof(float);

    glBindVertexArray(VAO[0]); // u sledecem dijelu koda koristim VAO 0
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]); // GL_ARRAY_BUFFER - buffer za pohranu podataka kao sto su  pozicije, normale, boje, koordinate teksture
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesName), verticesName, GL_STATIC_DRAW); // smjestas u GL_ARRAY_BUFFER, velicina, podaci, GL_STATIC_DRAW - podaci se nece cesto mijenjati

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, nameStride, ((void*)0)); //  postavlja parametre za jedan od atributa vertex shadera, ((void*)0) - pomjeraj u odnosu na pocetak
    glEnableVertexAttribArray(0); // da atribut na poziciji 0 postane aktivan
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, nameStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned nameTexture = loadImageToTexture(TITLE_TEXTURE_PATH);
    glBindTexture(GL_TEXTURE_2D, nameTexture); // u sledecem dijelu koda koristim ovu tekstru
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // ponavljaj teksturu
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //  piksel će uzeti boju najbližeg texela
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0); // unbindovanje trenutno aktivne teksture 

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

    unsigned mapTexture = loadImageToTexture(MAP_TEXTURE_PATH);
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Restricted Zone
    float verticesRestrictedZone[CIRCLE_RESOLUTION * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    generateCircle(CIRCLE_RESTRICTED_ZONE_CENTER_X, CIRCLE_RESTRICTED_ZONE_CENTER_Y, CIRCLE_RESTRICTED_ZONE_RADIUS, verticesRestrictedZone, CIRCLE_RESOLUTION);
    unsigned int circleStride = 2 * sizeof(float);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRestrictedZone), verticesRestrictedZone, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, circleStride, (void*)0);
    glEnableVertexAttribArray(0);

    // Airplanes
    float verticesFirstAirplane[CIRCLE_RESOLUTION * 2 + 4];
    generateCircle(FIRST_DRONE_INITIAL_X, FIRST_DRONE_INITIAL_Y, CIRCLE_DRONE_RADIUS, verticesFirstAirplane, CIRCLE_RESOLUTION);

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFirstAirplane), verticesFirstAirplane, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, circleStride, (void*)0);
    glEnableVertexAttribArray(0);
    
    float verticesSecondAirplane[CIRCLE_RESOLUTION * 2 + 4];
    generateCircle(SECOND_DRONE_INITIAL_X, SECOND_DRONE_INITIAL_Y, CIRCLE_DRONE_RADIUS, verticesSecondAirplane, CIRCLE_RESOLUTION);

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSecondAirplane), verticesSecondAirplane, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, circleStride, (void*)0);
    glEnableVertexAttribArray(0);

    // Indicators
    float verticesIndicators[] =
    {
        INDICATOR_LEFT, INDICATOR_BOTTOM,
        INDICATOR_LEFT, INDICATOR_TOP,
        INDICATOR_RIGHT, INDICATOR_BOTTOM,
        INDICATOR_RIGHT, INDICATOR_TOP,
    };
    unsigned int indicatorStride = 2 * sizeof(float);

    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesIndicators), verticesIndicators, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, indicatorStride, (void*)0);
    glEnableVertexAttribArray(0);

    // Progress Bars
    float verticesProgressBar[] = {
        PROGRESS_BAR_LEFT, PROGRESS_BAR_BOTTOM,                         
        PROGRESS_BAR_LEFT, PROGRESS_BAR_TOP,                            
        PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_BOTTOM,    
        PROGRESS_BAR_LEFT + PROGRESS_BAR_WIDTH, PROGRESS_BAR_TOP,       
    };
    unsigned int progressBarStride = 2 * sizeof(float);

    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesProgressBar), verticesProgressBar, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, progressBarStride, (void*)0);
    glEnableVertexAttribArray(0);

    // Background
    float verticesBackground[] = {
        INFO_LEFT, INFO_BOTTOM,   0.0, 0.0,    
        INFO_LEFT, INFO_TOP,      0.0, 1.0,    
        INFO_RIGHT, INFO_BOTTOM,  1.0, 0.0,    
        INFO_RIGHT, INFO_TOP,     1.0, 1.0,  
    };
    unsigned int infoStride = (2 + 2) * sizeof(float);
    
    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBackground), verticesBackground, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, infoStride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, infoStride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned firstDroneInfoText = loadImageToTexture(FIRST_DRONE_INFO_PATH);
    glBindTexture(GL_TEXTURE_2D, firstDroneInfoText);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned secondDroneInfoText = loadImageToTexture(SECOND_DRONE_INFO_PATH);
    glBindTexture(GL_TEXTURE_2D, secondDroneInfoText);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 3D

    Shader basic3dShader("basic_3d.vert", "basic_3d.frag");
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    // params - vertikalni ugao vidnog polja kamere, odnos širine i visine prozora ili ekrana, objekti blizi/dalji od bliske/daleke odsjecne ravni se odsjecaju
    
    glm::mat4 firstCameraView; // za tranformaciju objekta u odnosu na kameru
    glm::vec3 firstCameraPosition = glm::vec3(FIRST_DRONE_INITIAL_X, FIRST_DRONE_INITIAL_HEIGHT, FIRST_DRONE_INITIAL_Y + 1.0f);
    glm::vec3 firstCameraFront = glm::vec3(0.0f, 0.0f, 0.1f); // vektor koji odredjuje smijer gledanja kamere
    glm::vec3 firstCameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // znaci da je "gore" usmjereno prema pozitivnom y-smjeru (oznacava sta je gore u odnosu na kameru)
    float firstCameraYaw = INITIAL_CAMERA_ANGLE; // vrijednost "skretanja" koliko idemo gledamo ulijevo ili udesno po horizontali (yaw value which represents the magnitude we're looking to the left or to the right)
    float firstCameraPitch = 0.0f; // koliko gledamo gore/dole (angle that depicts how much we're looking up or down)

    glm::mat4 secondCameraView;
    glm::vec3 secondCameraPosition = glm::vec3(SECOND_DRONE_INITIAL_X, SECOND_DRONE_INITIAL_HEIGHT, SECOND_DRONE_INITIAL_Y + 1.0f);
    glm::vec3 secondCameraFront = glm::vec3(0.0f, 0.0f, 0.1f);
    glm::vec3 secondCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float secondCameraYaw = INITIAL_CAMERA_ANGLE;
    float secondCameraPitch = 0.0f;
    
    Model map("res/map.obj");
    Model firstDroneModel("res/drone.obj");
    Model secondDroneModel("res/drone.obj");

    // Postavi svjetlo i projekciju
    basic3dShader.use();
    basic3dShader.setVec3("uLightPos", 0, 1, 3);
    basic3dShader.setVec3("uViewPos", 0, 0, 5); // TODO: Sta sa ovim? Posto mi se kamera krece onda bih trebala i njega uvijek definisati zar ne?
    basic3dShader.setVec3("uLightColor", 1, 1, 1);
    basic3dShader.setMat4("uP", projection);

    basic3dShader.setVec3("uPointLights[0].color", 1.0f, 0.0f, 0.0f);
    basic3dShader.setVec3("uPointLights[1].color", 0.0f, 1.0f, 0.0f);
    basic3dShader.setVec3("uPointLights[2].color", 1.0f, 1.0f, 1.0f);
    basic3dShader.setVec3("uPointLights[3].color", 1.0f, 0.0f, 0.0f);
    basic3dShader.setVec3("uPointLights[4].color", 0.0f, 1.0f, 0.0f);
    basic3dShader.setVec3("uPointLights[5].color", 1.0f, 1.0f, 1.0f);
    
    setFront(firstCameraFront, firstCameraPitch, firstCameraYaw);
    firstCameraView = lookAt(firstCameraPosition, firstCameraPosition + firstCameraFront, firstCameraUp);
    basic3dShader.setMat4("uV", firstCameraView);
 
    setFront(secondCameraFront, secondCameraPitch, secondCameraYaw);
    secondCameraView = lookAt(secondCameraPosition, secondCameraPosition + secondCameraFront, secondCameraUp);
    basic3dShader.setMat4("uV", secondCameraView);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.84f, 0.93f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Keys Events
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (isFirstDroneActive)
        {
            // Prva kamera
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                firstCameraPosition += SPEED * glm::vec3(firstCameraFront.x, 0.0f, firstCameraFront.z);
            // pomjeranje unaprijed, odnosno u smijeru u koji kamera gleda, firstCameraFront je vektor koji označava smjer gledanja kamere,
            // i njega mnozim sa brzinom kretanja, pa to dodam na trenutnu poziciju kamere
            // NAPOMENA:  da bih zadrzala zeljeni pravac, moram da uzmem x i z koordinate fronta, ali z moram da vratim na 0 da
            // ne bi moja kamera pocela da se krece prema zemlji i od zemlje umjesto naprijed i nazad
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                firstCameraPosition -= SPEED * glm::vec3(firstCameraFront.x, 0.0f, firstCameraFront.z);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                firstCameraPosition -= glm::normalize(glm::cross(firstCameraFront, firstCameraUp)) * SPEED;
            // pomjeranje ulijeo, glm::cross(firstCameraFront, firstCameraUp)` daje vektor normalan na povrsinu
            // cross - daje vektorski proizvod, taj proizvod je ustvari normala na povrsinu
            // tu normalu normalizujemo (0-1) i onda taj vektor mnozimo sa brzinom kretanja
            // onda to sve oduzmemo/ dodamo u zavisnosti od kretanja lijevo/desno od trenutne pozicije
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                firstCameraPosition += glm::normalize(glm::cross(firstCameraFront, firstCameraUp)) * SPEED; 
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                isFirstDroneOnLand = false;
                if (firstCameraPosition.y <= DRONE_MAX_HEIGHT) firstCameraPosition  += SPEED * firstCameraUp;
            }
            // posto cameraUp oznacava sta je inad kamera (u ovom slucaju penjemo se po y osi, onda cameraUp ima (0,1,0) vrijednosti
            // onda to mnozimo s brzinom i dodajemo na poziciju kamere
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                firstCameraPosition -= SPEED * firstCameraUp;
                if (firstCameraPosition.y < DRONE_MIN_HEIGHT) destroyDrone(1);
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                firstCameraYaw -= CAMERA_SENSITIVITY;
            // posto yaw oznacava koliko idemo lijevo i desno na trenutnoj ravni, horizontalno, onda samo dodam/oduzmem ako hocu da vrsim rotaciju kamere
            // yaw potreban kasnije u kodu za izracunavanje novog cameraFront
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                firstCameraYaw += CAMERA_SENSITIVITY;
        }

        if (isSecondDroneActive)
        {
            // Druga kamera
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                secondCameraPosition += SPEED * glm::vec3(secondCameraFront.x, 0.0f, secondCameraFront.z);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                secondCameraPosition -= SPEED * glm::vec3(secondCameraFront.x, 0.0f, secondCameraFront.z);
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                secondCameraPosition -= glm::normalize(glm::cross(secondCameraFront, secondCameraUp)) * SPEED;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                secondCameraPosition += glm::normalize(glm::cross(secondCameraFront, secondCameraUp)) * SPEED;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                isSecondDroneOnLand = false;
                if (secondCameraPosition.y <= DRONE_MAX_HEIGHT) secondCameraPosition += SPEED * secondCameraUp;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                secondCameraPosition -= SPEED * secondCameraUp;
                if (secondCameraPosition.y < DRONE_MIN_HEIGHT) destroyDrone(2);
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                secondCameraYaw -= CAMERA_SENSITIVITY;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                secondCameraYaw += CAMERA_SENSITIVITY;
            }
        }

        // Check Errors
        if (isOutOfMap(firstCameraPosition) || isInRestricted(firstCameraPosition)) destroyDrone(1);
        if (isOutOfMap(secondCameraPosition) || isInRestricted(secondCameraPosition)) destroyDrone(2);
        if (isColision(firstCameraPosition, secondCameraPosition)) {
            destroyDrone(1);
            destroyDrone(2);
        }
        
        // On/Off Drone
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !isFirstDroneDestroyed) turnOnDrone(1);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) turnOffDrone(1);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !isSecondDroneDestroyed) turnOnDrone(2);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) turnOffDrone(2);

        // On/Off Camera
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !isFirstDroneDestroyed) turnOnCamera(1);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) turnOffCamera(1);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && !isSecondDroneDestroyed) turnOnCamera(2);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) turnOffCamera(2);


        // Consumption progress
        if (firstDroneConsumptionProgress >= 0) firstDroneConsumptionProgress -= firstDroneConsumption;
        else {
            if (isFirstDroneActive) destroyDrone(1); // ako dok je aktivan izgubi bateriju unisti se
            else {
                turnOffDrone(1); // ako nije aktivan, ako je samo upaljena kameraonda samo iskljuci drona
                turnOffCamera(1);
            }
        }
        if (secondDroneConsumptionProgress >= 0) secondDroneConsumptionProgress -= secondDroneConsumption;
        else {
            if (isSecondDroneActive) destroyDrone(2);
            else {
                turnOffDrone(2);
                turnOffCamera(2);
            }
        }
        
        // Land Drone
        if (!isFirstDroneActive && !isFirstDroneOnLand) {
            firstCameraPosition -= LAND_SPEED * firstCameraUp;
            if (firstCameraPosition.y <= DRONE_MIN_HEIGHT) isFirstDroneOnLand = true;
        }
        if (!isSecondDroneActive && !isSecondDroneOnLand) {
            secondCameraPosition -= LAND_SPEED * secondCameraUp;
            if (secondCameraPosition.y <= DRONE_MIN_HEIGHT) isSecondDroneOnLand = true;
        }
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 3D Render

        // Set lights
        draw3D();
        basic3dShader.use();
        if (isFirstDroneActive)
        {
            basic3dShader.setVec3("uPointLights[0].position", firstCameraPosition.x - DRONE_OUTBOX_WIDTH / 2, firstCameraPosition.y, firstCameraPosition.z);
            basic3dShader.setVec3("uPointLights[1].position", firstCameraPosition.x + DRONE_OUTBOX_WIDTH / 2, firstCameraPosition.y, firstCameraPosition.z);
            basic3dShader.setVec3("uPointLights[2].position", firstCameraPosition.x, firstCameraPosition.y, firstCameraPosition.z + DRONE_OUTBOX_WIDTH / 2);
            basic3dShader.setFloat("uPointLights[0].diffuseStrength", 0.4f);
            basic3dShader.setFloat("uPointLights[1].diffuseStrength", 0.4f);
            basic3dShader.setFloat("uPointLights[2].diffuseStrength", 0.2f);
        } else {
            basic3dShader.setFloat("uPointLights[0].diffuseStrength", 0.0f);
            basic3dShader.setFloat("uPointLights[1].diffuseStrength", 0.0f);
            basic3dShader.setFloat("uPointLights[2].diffuseStrength", 0.0f);
        }
        if (isSecondDroneActive)
        {
            basic3dShader.setVec3("uPointLights[3].position", secondCameraPosition.x - DRONE_OUTBOX_WIDTH / 2, secondCameraPosition.y, secondCameraPosition.z);
            basic3dShader.setVec3("uPointLights[4].position", secondCameraPosition.x + DRONE_OUTBOX_WIDTH / 2, secondCameraPosition.y, secondCameraPosition.z);
            basic3dShader.setVec3("uPointLights[5].position", secondCameraPosition.x, secondCameraPosition.y, secondCameraPosition.z + DRONE_OUTBOX_WIDTH / 2);
            basic3dShader.setFloat("uPointLights[3].diffuseStrength", 0.4f);
            basic3dShader.setFloat("uPointLights[4].diffuseStrength", 0.4f);
            basic3dShader.setFloat("uPointLights[5].diffuseStrength", 0.2f);
        } else {
            basic3dShader.setFloat("uPointLights[3].diffuseStrength", 0.0f);
            basic3dShader.setFloat("uPointLights[4].diffuseStrength", 0.0f);
            basic3dShader.setFloat("uPointLights[5].diffuseStrength", 0.0f);
        }
        
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Top Window
        topViewport();
        if (isFirstDroneCameraActive)
        {
            draw3D();
            basic3dShader.use();
            
            setFront(firstCameraFront, firstCameraPitch, firstCameraYaw);
            firstCameraView = lookAt(firstCameraPosition, firstCameraPosition + firstCameraFront, firstCameraUp);
            basic3dShader.setMat4("uV", firstCameraView);
            basic3dShader.setVec3("uViewPos", firstCameraPosition.x, firstCameraPosition.y, firstCameraPosition.z); 

            basic3dShader.setBool("uHasSpecular", true);
            basic3dShader.setMat4("uM", glm::mat4(1.0f)); // Prikaz mape za prvu kameru
            map.Draw(basic3dShader);
            
            // Prikaz drugog aviona za prvu mapu
            if (!isSecondDroneDestroyed) {
                glm::mat4 model = glm::mat4(1.0f);
                moveTo(model, secondCameraPosition.x, secondCameraPosition.y, secondCameraPosition.z);
                rotateTo(model, secondCameraYaw);
                basic3dShader.setBool("uHasSpecular", false);
                basic3dShader.setMat4("uM", model);
                secondDroneModel.Draw(basic3dShader);
            }
        } else {
            draw2D();
            glUseProgram(textureShader);
            glBindVertexArray(VAO[7]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, firstDroneInfoText);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 
        }

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Bottom Window
        bottomViewport();
        if (isSecondDroneCameraActive)
        {
            draw3D();
            basic3dShader.use();
            setFront(secondCameraFront, secondCameraPitch, secondCameraYaw);
            secondCameraView = glm::lookAt(secondCameraPosition, secondCameraPosition + secondCameraFront, secondCameraUp);
            basic3dShader.setMat4("uV", secondCameraView);
            basic3dShader.setVec3("uViewPos", secondCameraPosition.x, secondCameraPosition.y, secondCameraPosition.z); 

            basic3dShader.setBool("uHasSpecular", true);
            basic3dShader.setMat4("uM", glm::mat4(1.0f));
            map.Draw(basic3dShader);
        
            // Prikaz prvog aviona za drugu mapu
            if (!isFirstDroneDestroyed) {
                glm::mat4 model = glm::mat4(1.0f);
                moveTo(model, firstCameraPosition.x, firstCameraPosition.y, firstCameraPosition.z);
                rotateTo(model, firstCameraYaw);
                basic3dShader.setBool("uHasSpecular", false);
                basic3dShader.setMat4("uM", model);
                firstDroneModel.Draw(basic3dShader);
            }
        } else {
            draw2D();
            glUseProgram(textureShader);
            glBindVertexArray(VAO[7]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, secondDroneInfoText);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 
        }

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
        glBindVertexArray(VAO[1]); // u nastavku koda koristim VAO 1
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, mapTexture); // u nastavku koda koristim ovu teksturu
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // already active texture shader
        glUseProgram(basicShader);
        glUniform4f(uBasicShaderColor, 0.6f, 0.9f, 0.5f, 0.25f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Restricted Zone
        glBindVertexArray(VAO[2]);
        glUniform4f(uBasicShaderColor, 1.0f, 0.0f, 0.0f, 0.2f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(verticesRestrictedZone) / (2 * sizeof(float)));

        // Airplanes
        glUseProgram(droneShader);

        if (!isFirstDroneDestroyed) {
            float scale = mapValue(firstCameraPosition.y, DRONE_MIN_HEIGHT, DRONE_MAX_HEIGHT, 0.15f, 0.23f);
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(firstCameraPosition.x, -firstCameraPosition.z, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, 0.0f));
            glUniformMatrix4fv(uDroneShaderModelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(verticesFirstAirplane) / (2 * sizeof(float)));
        }

        if (!isSecondDroneDestroyed) {
            float scale = mapValue(secondCameraPosition.y, DRONE_MIN_HEIGHT, DRONE_MAX_HEIGHT, 0.15f, 0.23f);
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(secondCameraPosition.x, -secondCameraPosition.z, 0.0f));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(scale, scale, 0.0f));
            glUniformMatrix4fv(uDroneShaderModelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(verticesSecondAirplane) / (2 * sizeof(float)));
        }

        // Indicators
        // Top
        topViewport();
        glUseProgram(basicShader);
        glBindVertexArray(VAO[5]);
        if (isFirstDroneActive) glUniform4f(uBasicShaderColor, INDICATOR_R, INDICATOR_G, INDICATOR_B, 1.0);
        else glUniform4f(uBasicShaderColor, INACTIVE_INDICATOR_R, INACTIVE_INDICATOR_G, INACTIVE_INDICATOR_B, 1.0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(VAO[6]);
        glUniform4f(uBasicShaderColor, DRONE_R, DRONE_G, DRONE_B, 0.4f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(progressBarShader);
        glUniform1f(uProgressBarShaderProgress, firstDroneConsumptionProgress);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Bottom
        bottomViewport();
        glUseProgram(basicShader);
        glBindVertexArray(VAO[5]);
        if (isSecondDroneActive) glUniform4f(uBasicShaderColor, INDICATOR_R, INDICATOR_G, INDICATOR_B, 1.0);
        else glUniform4f(uBasicShaderColor, INACTIVE_INDICATOR_R, INACTIVE_INDICATOR_G, INACTIVE_INDICATOR_B, 1.0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(VAO[6]);
        glUniform4f(uBasicShaderColor, DRONE_R, DRONE_G, DRONE_B, 0.4f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(progressBarShader);
        glUniform1f(uProgressBarShaderProgress, secondDroneConsumptionProgress);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Delete Textures
    glDeleteTextures(1, &nameTexture);
    glDeleteTextures(1, &mapTexture);
    glDeleteTextures(1, &firstDroneInfoText);
    glDeleteTextures(1, &secondDroneInfoText);

    // Delete VBO and VAO
    glDeleteBuffers(9, VBO);
    glDeleteVertexArrays(9, VAO);

    // Delete shaders
    glDeleteProgram(basicShader);
    glDeleteProgram(textureShader);
    glDeleteProgram(droneShader);
    glDeleteProgram(progressBarShader);
    
    glfwTerminate();
    return 0;
}