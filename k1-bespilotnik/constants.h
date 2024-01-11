#pragma once
#include <iostream>
#include <string>

constexpr const char* TITLE = "Bespilotnik";
constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 900;

constexpr int CIRCLE_RESOLUTION = 50;
constexpr float CIRCLE_PLANE_RADIUS = 0.03;
constexpr float FIRST_PLANE_CENTER_X = -0.7;
constexpr float FIRST_PLANE_CENTER_Y = -0.5;
constexpr float SECOND_PLANE_CENTER_X = 0.7;
constexpr float SECOND_PLANE_CENTER_Y = -0.5;
constexpr float FIRST_PLANE_R = 0.2;
constexpr float FIRST_PLANE_G = 0.5;
constexpr float FIRST_PLANE_B = 1.0;
constexpr float SECOND_PLANE_R = 0.2;
constexpr float SECOND_PLANE_G = 0.5;
constexpr float SECOND_PLANE_B = 1.0;
constexpr float PLANE_SPEED = 0.001;

constexpr float FIRST_INDICATOR_LEFT = -0.95;
constexpr float FIRST_INDICATOR_RIGHT = -0.85;
constexpr float FIRST_INDICATOR_TOP = -0.7;
constexpr float FIRST_INDICATOR_BOTTOM = -0.8;
constexpr float FIRST_INDICATOR_R = 0.4;
constexpr float FIRST_INDICATOR_G = 0.65;
constexpr float FIRST_INDICATOR_B = 0.0;
constexpr float SECOND_INDICATOR_R = 0.4;
constexpr float SECOND_INDICATOR_G = 0.65;
constexpr float SECOND_INDICATOR_B = 0.0;
constexpr float INACTIV_INDICATOR_R = 0.2;
constexpr float INACTIV_INDICATOR_G = 0.2;
constexpr float INACTIV_INDICATOR_B = 0.0;

constexpr float SECOND_INDICATOR_LEFT = 0.45;
constexpr float SECOND_INDICATOR_RIGHT = 0.55;
constexpr float SECOND_INDICATOR_TOP = -0.7;
constexpr float SECOND_INDICATOR_BOTTOM = -0.8;

constexpr float FIRST_PROGRESS_BAR_LEFT = -0.95;
constexpr float SECOND_PROGRESS_BAR_LEFT = 0.45;

constexpr float PROGRESS_BAR_TOP = -0.85;
constexpr float PROGRESS_BAR_BOTTOM = -0.95;
constexpr float PROGRESS_BAR_WIDTH = 0.5;
constexpr float PROGRESS_BAR_OFFSET = 0.0001;

constexpr float CIRCLE_RESTRICTED_ZONE_RADIUS = 0.35;
constexpr float CIRCLE_RESTRICTED_ZONE_CENTER_X = 0.1;
constexpr float CIRCLE_RESTRICTED_ZONE_CENTER_Y = 0.3;

constexpr float MAP_LEFT = -0.95;
constexpr float MAP_RIGHT = 0.95;
constexpr float MAP_TOP = 0.9;
constexpr float MAP_BOTTOM = -0.65;

constexpr float TITLE_LEFT = -0.95;
constexpr float TITLE_RIGHT = -0.6;
constexpr float TITLE_TOP = 0.99;
constexpr float TITLE_BOTTOM = 0.9;

constexpr const char* MAP_IMAGE_PATH = "textures/majevica-map.png";
constexpr const char* TITLE_IMAGE_PATH = "textures/name.png";