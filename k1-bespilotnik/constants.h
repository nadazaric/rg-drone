#pragma once
#include <iostream>
#include <string>

// Global
constexpr const char* TITLE = "Bespilotnik";
constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 1100;

// Camera
constexpr float CAMERA_SENSITIVITY = 0.2f;
constexpr float CAMERA_SPEED = 0.001f;
constexpr float CAMERA_ANGLE = 30.0f;

constexpr int CIRCLE_RESOLUTION = 50;
constexpr float CIRCLE_PLANE_RADIUS = 0.03;
constexpr float FIRST_AIRPLANE_INITIAL_X = -0.7f;
constexpr float FIRST_AIRPLANE_INITIAL_Y = -0.5f;
constexpr float FIRST_AIRPLANE_INITIAL_HEIGHT = 0.1f;
constexpr float SECOND_AIRPLANE_INITIAL_X = 0.7f;
constexpr float SECOND_AIRPLANE_INITIAL_Y = -0.5f;
constexpr float SECOND_AIRPLANE_INITIAL_HEIGHT = 0.1f;
constexpr float FIRST_PLANE_R = 0.2;
constexpr float FIRST_PLANE_G = 0.5;
constexpr float FIRST_PLANE_B = 1.0;
constexpr float SECOND_PLANE_R = 0.2;
constexpr float SECOND_PLANE_G = 0.5;
constexpr float SECOND_PLANE_B = 1.0;
constexpr float PLANE_SPEED = 0.001;

constexpr float INDICATOR_LEFT = -0.85f;
constexpr float INDICATOR_RIGHT = -0.95f;
constexpr float INDICATOR_TOP = -0.75f;
constexpr float FIRST_INDICATOR_BOTTOM = -0.95f;
constexpr float INDICATOR_R = 0.2f;
constexpr float INDICATOR_G = 0.7f;
constexpr float INDICATOR_B = 0.3f;
constexpr float INACTIVE_INDICATOR_R = 0.0f;
constexpr float INACTIVE_INDICATOR_G = 0.03f;
constexpr float INACTIVE_INDICATOR_B = 0.0f;

constexpr float FIRST_PROGRESS_BAR_LEFT = -0.95;
constexpr float SECOND_PROGRESS_BAR_LEFT = 0.45;

constexpr float PROGRESS_BAR_TOP = -0.85;
constexpr float PROGRESS_BAR_BOTTOM = -0.95;
constexpr float PROGRESS_BAR_WIDTH = 0.5;
constexpr float PROGRESS_BAR_OFFSET = 0.0001;

constexpr float CIRCLE_RESTRICTED_ZONE_RADIUS = 0.42;
constexpr float CIRCLE_RESTRICTED_ZONE_CENTER_X = 0.0;
constexpr float CIRCLE_RESTRICTED_ZONE_CENTER_Y = 0.2;

constexpr float MAP_LEFT = -1;
constexpr float MAP_RIGHT = 1;
constexpr float MAP_TOP = 1;
constexpr float MAP_BOTTOM = -1;


constexpr unsigned int MAP_WIDTH = 300;
constexpr unsigned int MAP_HEIGHT = 300;

constexpr float TITLE_LEFT = -0.95;
constexpr float TITLE_RIGHT = -0.6;
constexpr float TITLE_TOP = 1;
constexpr float TITLE_BOTTOM = 0.92;

constexpr const char* MAP_IMAGE_PATH = "textures/majevica-map.jpg";
constexpr const char* TITLE_IMAGE_PATH = "textures/name.png";