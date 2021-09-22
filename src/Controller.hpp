#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp> 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>

#include "common.hpp"
#include "settings.hpp"
#include "gui.hpp"
#include "ModelLoader.hpp"
#include "Shader.hpp"
#include "View.hpp"
#include "GraphicObject.hpp"
#include "Materials.hpp"
#include "Match.hpp"
#include "Arrow.hpp"
#include "OffsideLine.hpp"




class Controller
{
public:
	Controller(char** argv);
	~Controller();
	void start();
	void runPerformanceTest(char** argv);

private:
	// app related
	void InitGLFW();
	void InitGLEW();
	void InitOpenGL(int w, int h);

	void InitDepthShader();

	static void ErrorCallback(int error, const char* description);
	static void ResizeWindow(GLFWwindow* window, int width, int height);
	static void MouseButtonnCallback(GLFWwindow* window, int button, int action, int mods);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// graphics 
	void ResizeField();
	glm::mat4 VideoInitializationAnimation();
	void RefineViewMatrix();
	bool GetChangeType(int type, glm::mat4 proj, glm::mat4 view, float sensitivity, double totalError, double totalErrorNew);
	float GetRotationToParallel(std::vector<cv::Point2f> line1, std::vector<cv::Point2f> line2);

	void CalibrationBestStep(glm::mat4 proj, glm::mat4 view, float& distance, float& rotX, float& posY, float* sensitivity, double& currentError);

	void LoadVideoFrame(bool tests = false);
	void SetGraphicObjects();
	void GenerateGraphicObjects();
	void MovePlayerHightlits(std::vector<std::vector<glm::vec2>> playerPositions);
	void RenderScene(Shader* shader);
	void RenderQuad();
	void LockFrameRate(double frame_rate, bool tests = false);
	void ApplyDemoSettings();


	View* viewHandle;

	Shader* _objectShader;
	Shader* _depthShader;
	Shader* _depthViewShader;
	GraphicObject* _circle;
	GraphicObject* _lightCube;
	GraphicObject* _soccerPitch;
	Arrow* _arrow;
	OffsideLine* _offsideLine;

	glm::vec4 _light = glm::vec4(0.f, -100.f, -18.f, 0.f);

	unsigned int depthMapFBO = 0;
	unsigned int depthMap = 0;
	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	// Frame counting and limiting
	double _frameStartTime, _frameEndTime, _frameDrawTime;
	struct InitVideoAnimationParams {
		float distance = 0.f;
		float rotX = 0.f;
		float posY = 0.f;
		float dd = 0.;
		float dx = 0.f;
		float dy = 0.f;
		double start;
		double end;
		double afterAnimationDelay = 1;
		bool running = false;
	};
	InitVideoAnimationParams videoAnimParams;

	double currentViewError = 0.0;
	const float initViewParams[3] = { 150.f, 90.f, 0.f };
	float _distance = 0.f, _rotX = 0.f, _posY = 0.f;
	int afterAnimationDelay = 0;

	// match 
	Match* match = nullptr;
	struct PerformanceUnit {
		std::vector<double> time;
		int cnt = 0;
	};
	std::string path;

	void PerformanceUnitAddSample(PerformanceUnit &unit, double time);
	double GetAverageUnitTime(PerformanceUnit unit);
};
