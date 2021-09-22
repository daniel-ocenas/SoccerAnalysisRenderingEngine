#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include "gui.hpp"
#include "common.hpp"
#include "settings.hpp"
#include "Shader.hpp"
#include "Match.hpp"

class VideoLoader
{
public:
	VideoLoader(std::string path);
	~VideoLoader();

	void InitNewVideo(std::string videoPath);
	bool InitVideoFrame(Gui& gui, bool tests = false);
	void LoadDemoVideoPoints(std::string videoPath);
	
	void DrawBackground();
	void DrawMaskFrame(int viewType);
	void DrawShadowsOnBackground(GLint frameBufferId);

	void ResizeVideo(int windowWidth, int windowHeight);

	void CalculateHomographyMatrix();
	void FindPlayersPositions(Match& match, Gui& gui);
	void TestViewMatrix(glm::mat4 projViewMatrix, double & totalError );

	int GetVideoWidth();
	int GetVideoHeight();
	double GetFPS();
	double GetTime() { return _frameCount / _videoFPS; };
	double GetFrameCount() { return _frameCount; };
	glm::mat4 GetTMatGLM();
	cv::Matx33f GetTMat33f() { return TMat; };
	glm::mat4 CalculateVideoViewMatrix();

	
	void SetFieldParametes(float width, float length);
	std::vector<cv::Point2f> GetPitchScreenPoints() { return pitchScreenPoints; }
	std::vector<cv::Point2f> GetProjectedPoints() { return projectedPoints; }
	cv::Point2f GetScreenToScenePoint(float xpos, float ypos);
	void AddPitchScreenPoint(int x, int y);
	void EditPitchScreenPoint(int index, int x, int y);
	void SetSliderVal1(float val) { scaleVal = val; }

	

private:	

	void CalcProjectedPointScreenPos( glm::mat4 projViewMatrix, cv::Point3f inModelPoint, cv::Point2f& outScreenPoint);

	void CreateTextureBuffers();
	void ClearTextures();
	void ClearTextureBuffers();
	void MOG2(cv::Mat& img, cv::Mat& mog2foregroundMask);
	void ChromaKey(cv::Mat& img, cv::Mat& result, int thLow, int thHigh);
	void FloodFill(cv::Mat& img, cv::Mat& result, int thLow, int thHigh);
	void MatToTexture(const cv::Mat& mat, GLuint& textureId, bool &initialized);


	void DrawTexture(GLint textureId, bool initialized);
	void DeleteTexture(GLuint textureId, bool& initialized);
	void CreatePlayerMask(cv::Mat in, cv::Mat& out);


	int _windowWidth = 1920;
	int _windowHeight = 1080;

	cv::Ptr<cv::BackgroundSubtractor> mog2BackgSubs;
	cv::Mat _kernelElement;

	cv::VideoCapture* _capture = nullptr;
	

	std::vector<cv::Point2f> projectedPoints;
	std::vector<cv::Point2f> pitchScreenPoints;


	cv::Mat _playerMask, frame;
	Shader* _shader = nullptr;
	Shader* _shaderBlend = nullptr;
	Shader* _shaderShadow = nullptr;
	GLuint _backgroundTextureLoc, _maskTextureLoc, _foregroundTextureLoc;
	GLuint VAO = 10, VBO = 11, EBO = 12;
	GLuint _backgroundTexture = 1;
	GLuint _thresholdTexture = 2;
	bool _thresholdTextureInitialized = false;
	bool _backgroundTextureInitialized = false;

	std::vector<cv::Point2f> midpoints;
	std::vector<cv::Point2f> modelCorners;
	cv::Matx33f TMat;
	glm::mat4 _videoViewMat = glm::mat4(1.f);
	
	bool _pauseDetected = false;
	bool _isPaused = false;
	bool _isResumed = false;
	bool _firstFrameLoaded = true;
	bool _maskCalculated = false;
	bool editingPitchPoints = false;
	double _videoFPS = 60;
	double _videoLastFrame = 0;
	int _frameCount = 0;

	float scaleVal = 2.7f;



};

 