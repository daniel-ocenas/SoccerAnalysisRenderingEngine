#pragma once
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "Materials.hpp"

// constants
static const unsigned int VIEWPORT_YOFFSET = 300;
static const unsigned int ORIGINALVIDEO = 0;
static const unsigned int THRESHOLDVIDEO = 1;
static const unsigned int MASKINGVIDEO = 2;

static const unsigned int ROT_X = 0;
static const unsigned int POS_Y = 1;
static const unsigned int DISTANCE = 2;
static const unsigned int LENGTH = 3;
static const unsigned int WIDTH = 4;

struct Vertex {
	float position[3];
};

struct Normal {
	float position[3];
};

struct UV {
	float position[2];
};


// Color thresholds
// colors list
struct Color {
	unsigned int colorId;
	const char* name;
	const cv::Scalar lowTh;
	const cv::Scalar highTh;
	const Material material;
	const int type;
};

enum COLORS {
	Blue, White, Red, Orange, Green, Yellow, Gold, Pink
};

const Color colors[] = {
	{ 0,"Blue", cv::Scalar(110, 50, 50), cv::Scalar(130, 255, 255),BLUE, cv::COLOR_BGR2HSV},
	{ 1,"White", cv::Scalar(0, 240, 0), cv::Scalar(255, 255, 255), WHITE, cv::COLOR_BGR2HLS},
	{ 2,"Red", cv::Scalar(0, 50, 50), cv::Scalar(25, 255, 255), RED, cv::COLOR_BGR2HSV},
	{ 3,"Orange", cv::Scalar(13, 50, 50), cv::Scalar(28, 255, 255),ORANGE, cv::COLOR_BGR2HSV},
	{ 4,"Green", cv::Scalar(40, 50, 50), cv::Scalar(65, 255, 255), EMERALD, cv::COLOR_BGR2HSV},
	{ 5,"Yellow", cv::Scalar(25, 50, 50), cv::Scalar(35, 255, 255),YELLOW,  cv::COLOR_BGR2HSV},
	{ 6,"Gold", cv::Scalar(25, 50, 50), cv::Scalar(35, 255, 255), GOLD,  cv::COLOR_BGR2HSV},
	{ 7,"Pink", cv::Scalar(140, 50, 50), cv::Scalar(160, 255, 255), MAGENTA, cv::COLOR_BGR2HSV},
};

