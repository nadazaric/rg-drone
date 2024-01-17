#pragma once
#include <iostream>
#include <string>

// Global
constexpr const char* TITLE = "Bespilotnik";
constexpr int WINDOW_WIDTH = 1100;
constexpr int WINDOW_HEIGHT = 1100;
constexpr int CIRCLE_RESOLUTION = 50;

// Name
constexpr float NAME_LEFT = -0.95f;
constexpr float NAME_RIGHT = -0.6f;
constexpr float NAME_TOP = 1.0f;
constexpr float NAME_BOTTOM = 0.92f;

// Map
constexpr float MAP_LEFT = -1.0f;
constexpr float MAP_RIGHT = 1.0f;
constexpr float MAP_TOP = 1.0f;
constexpr float MAP_BOTTOM = -1.0f;
constexpr unsigned int MAP_WIDTH = 300;
constexpr unsigned int MAP_HEIGHT = 300;

// Restricted Zone
constexpr float CIRCLE_RESTRICTED_ZONE_RADIUS = 0.42f;
constexpr float CIRCLE_RESTRICTED_ZONE_CENTER_X = 0.0f;
constexpr float CIRCLE_RESTRICTED_ZONE_CENTER_Y = 0.2f;

// Camera
constexpr float CAMERA_SENSITIVITY = 0.7f;
constexpr float INITIAL_CAMERA_ANGLE = -90.0f;
constexpr float SPEED = 0.01f;
constexpr float LAND_SPEED = 0.005f;
constexpr float CAMERA_ANGLE = 30.0f;

// Indicators
constexpr float INDICATOR_LEFT = -0.85f;
constexpr float INDICATOR_RIGHT = -0.95f;
constexpr float INDICATOR_TOP = -0.75f;
constexpr float INDICATOR_BOTTOM = -0.95f;
constexpr float INDICATOR_R = 0.2f;
constexpr float INDICATOR_G = 0.7f;
constexpr float INDICATOR_B = 0.3f;
constexpr float INACTIVE_INDICATOR_R = 0.0f;
constexpr float INACTIVE_INDICATOR_G = 0.03f;
constexpr float INACTIVE_INDICATOR_B = 0.0f;

// Drones
constexpr float CIRCLE_DRONE_RADIUS = 0.05f;
constexpr float DRONE_OUTBOX_WIDTH = 0.15f;
constexpr float DRONE_OUTBOX_LENGTH = 0.15f;
constexpr float DRONE_OUTBOX_HEIGHT = 0.05f;
constexpr float DRONE_MIN_HEIGHT = 0.1f;
constexpr float DRONE_MAX_HEIGHT = 1.0f;
constexpr float DRONE_R = 0.2f;
constexpr float DRONE_G = 0.5f;
constexpr float DRONE_B = 1.0f;
constexpr float FIRST_DRONE_INITIAL_X = -0.7f;
constexpr float FIRST_DRONE_INITIAL_Y = -0.5f;
constexpr float FIRST_DRONE_INITIAL_HEIGHT = 0.1f;
constexpr float SECOND_DRONE_INITIAL_X = 0.7f;
constexpr float SECOND_DRONE_INITIAL_Y = -0.5f;
constexpr float SECOND_DRONE_INITIAL_HEIGHT = 0.1f;

// Progress Bar
constexpr float PROGRESS_BAR_TOP = -0.75f;
constexpr float PROGRESS_BAR_BOTTOM = -0.95f;
constexpr float PROGRESS_BAR_LEFT = -0.8f;
constexpr float PROGRESS_BAR_WIDTH = 0.5;
constexpr float CONSUMPTION_DRONE_ACTIVE = 0.00008f;
constexpr float CONSUMPTION_CAMERA_ON = 0.00002f;

// Info text
constexpr float INFO_LEFT = -1.0f;
constexpr float INFO_RIGHT = 1.0f;
constexpr float INFO_TOP = 1.0f;
constexpr float INFO_BOTTOM = -1.0f;

// Textures
constexpr const char* MAP_TEXTURE_PATH = "textures/majevica-map.jpg";
constexpr const char* TITLE_TEXTURE_PATH = "textures/name.png";
constexpr const char* FIRST_DRONE_INFO_PATH = "textures/drone1_info_camera.png";
constexpr const char* SECOND_DRONE_INFO_PATH = "textures/drone2_info_camera.png";