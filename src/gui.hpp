#pragma once


#include <iostream>
#include <cstdlib>
#include <string>
#include <imgui.h>
#include <vector>
#include <stdlib.h> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h> 
#include <imgui_internal.h> 
#include <ImGuiFileDialog.h>  
#include <opencv2/opencv.hpp>

#include "settings.hpp"
#include "common.hpp"
#include "Team.hpp"
#include "Arrow.hpp"
#include "OffsideLine.hpp"


struct Icon {
	GLuint textureId;
	bool boolean;
	int width;
	int height;
};


class Gui
{

public:
	Gui();
	~Gui();
	void Init(GLFWwindow* window, std::string path);
	void NewFrame(double videoTime);
	void CleanUp();
	void Render();

	void DebugWindowDraw();
	void ArrowMenu(double videoTime, Arrow &arrows);
	void MainMenu(double videoTime, GraphicObject &plane, double viewError);
	void MatchMenu(GraphicObject& highlits, Arrow& arrows);
	void OffsideLineMenu(double videoTime, OffsideLine& plane);
	void NewVideoMenu(double viewError);
	void AddArrowsList(glm::ivec2 p1, glm::ivec2 p2, double startTime);
	void InitializeTextureIcon(std::string location, Icon& textureID);

	// functions for obtaining gui input parameters
	int GetSliderVal(int index);
	int GetFieldLength() { return fieldLength; };
	int GetFieldWidth() { return fieldWidth; };
	int GetViewType() { return _imgView; };
	int GetMatrixType() { return _matView; };	
	int GetMouseSelect() { return _mouseSelect; };
	int GetCalibrationType() { return _calibration; };
	int GetArrowNewColor() { return arrowNewColor; };
	int GetSelectedCornerIndex() { return selectedCornerIndex; };
	int GetSelectedPitchPointsCount() { return selectedPitchPointsCount; };
	int GetTeam1Color() { return teamColor[0]; };
	int GetTeam2Color() { return teamColor[1]; };
	bool GetEditingPitchPoints() { return editingPitchPoints; };
	bool GetClearPitchPoints() { return clearPitchPoints; };
	bool GetDragging() { return dragging; };
	bool GetVideoSelectMenuConfirmed() { return videoSelectMenuConfirmed; };
	bool GetVideoState() { return play.boolean; };
	bool GetPlayerHighliting() { return _playerHighlighting; };
	bool GetArrow() { return _arrowDrawing; };
	bool GetPlaneShow() { return _showPlane; };	
	bool GetOffsideLineMove() { return _offsideLineMove; };
	bool GetArrowCreation() { return _arrowInCreation; };
	bool GetArrowPointSelection() { return _arrowPointSelection; };
	bool GetNewVideoFileSelected() { return _newVideoFileSelected; };
	bool GetNewTeamColorSelected() { return _newTeamColorSelected; };
	bool GetVideoFileUsed() { return _videoFileUsed; };
	bool GetVideoFileEnded() { return _videoFileEnded; };
	bool GetRecalculateProjections() { return _recalculateProjections; };
	bool GetSavedSettings() { return savedSettings; };
	std::string GetFilePathName() { return _filePathName;};
	

	void SetArrowCreation(bool value) { _arrowInCreation = value; };
	void SetArrowPointSelection(bool value) { _arrowPointSelection = value; };
	void SetClearPitchPoints(bool value) { clearPitchPoints = value; };
	void SetDragging(bool value) { dragging = value; };
	void SetEditingPitchPoints(bool value) { editingPitchPoints = value; };
	void SetFieldLength(int value) { fieldLength = value; };
	void SetFieldWidth(int value) { fieldWidth = value; };
	void SetFilePathName(std::string pathName) { _filePathName = pathName; };
	void SetMatrixType(int view) { _matView = view; };
	void SetMouseSelect(int view) { _mouseSelect = view; };
	void SetNewVideoFileSelected(bool value) { _newVideoFileSelected = value; };
	void SetNewTeamColorSelected(bool value) { _newTeamColorSelected = value; };
	void SetOffsideLineMove(bool value) { _offsideLineMove = value; };
	void SetPoint1(float x, float y) { p1 = { x, y }; };
	void SetPoint2(float x, float y) { p2 = { x, y }; };
	void SetPlay(bool paused) { play.boolean = paused; };
	void SetPlayInvert() { play.boolean = !play.boolean; };
	void SetPlaneShow(bool show) { _showPlane = show; };
	void SetRecalculateProjections(bool value) { _recalculateProjections = value; };
	void SetSelectedCornerIndex(int value) { selectedCornerIndex = value; };
	void SetSelectedPitchPointsCount(int num) { selectedPitchPointsCount = num; };
	void SetVideoFileUsed(bool value) { _videoFileUsed = value; };
	void SetVideoFileEnded(bool value) { _videoFileEnded = value; };
	void SetVideoSelectMenuConfirmed(bool value) { videoSelectMenuConfirmed = value; };
	void SetSavedSettings(bool value) { savedSettings = value; };
	void SetWindowSize(float w, float h) { _windowSize.x = w; _windowSize.y = h; };
	glm::vec2 p1 = glm::vec2(0, 0);
	glm::vec2 p2 = glm::vec2(0, 0);

private:
	void OpenVideoFunc(GraphicObject &soccerPitch);
	std::string _path;
	int openDemoVideo = 0;
	int sliderSize = 3;
	std::vector<int> _slider;

	std::string _filePathName;
	ImVec2 _windowSize;
	bool _firstFrame = true;

	int fieldWidth = 64; 
	int fieldLength = 100;


	int _mouseSelect = 0;
	int _imgView = MASKINGVIDEO;
	int _matView = 0;
	int _calibration = 2;

	bool _showPlane = true;
	bool _playerHighlighting = true;
	bool _arrowDrawing = true;
	bool _offsideLineMove = false;

	bool _arrowInCreation = false;
	bool _arrowPointSelection = false;

	//bool pitchPointsInit = true;
	bool _newVideoFileSelectedMenu = false;
	bool _newVideoFileSelected = false;
	bool _videoFileUsed = false;
	bool _videoFileEnded = false;

	bool _recalculateProjections = false;

	int selectedCornerIndex = 0;
	int selectedPitchPointsCount = 0;
	bool editingPitchPoints = false;
	bool clearPitchPoints = false;
	bool dragging = false;
	bool videoSelectMenuConfirmed = false;

	bool addArrow = false;
	int arrowNewColor = 0;
	int ArrowEdit = -1;

	bool pitchPointsInit = false;
	int teamColor[2] = {COLORS::Blue, COLORS::White};
	int teamSelectCombo[2] = { -1, -1 };
	bool _newTeamColorSelected = false;

	bool savedSettings = false;

	Icon pause = { 0, true };
	Icon play = { 0, false };
	Icon circle = { 0, false };
	Icon replay = { 0, false };
	Icon edit = { 0, false };
	Icon remove = { 0, false };
	Icon flag = { 0, false };
	Icon arrow = { 0, false };


};

