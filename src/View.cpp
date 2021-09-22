#include "View.hpp"

/**
* @brief View Constructor
*/
View::View(GLFWwindow* window)
{
	_window = window;
	glfwGetWindowSize(_window, &width, &height);

	glfwSetWindowUserPointer(_window, reinterpret_cast<void*>(this));

	_perspectiveCamera = new PerspectiveCamera(45.f, (float)width / height, 1.f, 20000.f);

	gui = new Gui();
}


/**
* @brief View Destructor
*/
View::~View()
{

	if (gui != nullptr)
	{
		gui->CleanUp();
		delete gui;
	}

	if (_video != nullptr)
		delete _video;

	if (_perspectiveCamera != nullptr)
		delete _perspectiveCamera;

	if (_cursor != nullptr)
		glfwDestroyCursor(_cursor);

	if (_window != nullptr)
		glfwDestroyWindow(_window);


}


/**
* @brief Function initiates video
*/
void View::InitVideoLoader(std::string path)
{
	_video = new VideoLoader(path);
}