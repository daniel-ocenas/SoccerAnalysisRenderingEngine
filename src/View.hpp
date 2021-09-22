#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "PerspectiveCamera.hpp"
#include "gui.hpp"
#include "Videoloader.hpp"

class View
{
public:
	View(GLFWwindow* window);
	~View();

	GLFWwindow* GetWindow() { return _window; }
	PerspectiveCamera* GetPerspectiveCamera() { return _perspectiveCamera; }
	VideoLoader* GetVideoHandler() { return _video; }
	Gui* GetGui() { return gui; };

	void setCursor(GLFWcursor* newCursor) { _cursor = newCursor; };
	void InitVideoLoader(std::string path);

	int width;
	int height;
	int monitorWidth;
	int monitorHeight;
	float moveSensitivity = 1.f;

private:
	PerspectiveCamera* _perspectiveCamera = nullptr;
	GLFWwindow* _window = nullptr;
	VideoLoader* _video = nullptr;
	GLFWcursor* _cursor = nullptr;

	Gui* gui = nullptr;
};

