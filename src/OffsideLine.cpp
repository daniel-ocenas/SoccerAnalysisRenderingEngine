#include "OffsideLine.hpp"


/**
* @brief OffsideLine constructor
*/
OffsideLine::OffsideLine(std::string path)
{
	ModelLoader loader;
	loader.loadObjFile(path + "\\graphics\\objects3D\\plane.obj", _offsideLine.vao, _offsideLine.mesh);
	//loader.loadObjFile(_offsideLine.vao, _offsideLine.mesh, planeVertices, planeUVs, planeNormals, planeSize);

	// generate object instance
	_offsideLine.CreateObjectInstance(glm::mat4(0.f), REDPLASTIC);
	_offsideLine.instances.at(0).active = false;
	_offsideLine.instances.at(0).p1 = glm::vec2(0.f, 0.f);
}

/**
* @brief OffsideLine destructor
*/
OffsideLine::~OffsideLine()
{
}

/**
* @brief Function stores model matrix
*/
void OffsideLine::SetModelmat(float posX, float fieldWidth)
{
	_offsideLine.instances.at(0).p1 = glm::vec2(posX, 0.f);
	_offsideLine.instances.at(0).modelMat = glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(posX, .01f, 0.f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 1.f, fieldWidth / 2.f)));

}
/**
* @brief Function  return x position of last defender
*/
void OffsideLine::AutoOffside(std::vector<std::vector<glm::vec2>> playerPositions, float fieldWidth)
{
	if (_offsideLine.instances.at(0).p1.x != 0.f)
	{
		glm::vec2 min = glm::vec2(100.0f, 0.f);
		for (auto it = std::begin(playerPositions[0]); it != std::end(playerPositions[0]); ++it) {
			if (it->x < min.x)
				min.x = it->x;
		}

		glm::vec2 max = glm::vec2(-100.0f, 0.f);
		for (auto it = std::begin(playerPositions[1]); it != std::end(playerPositions[1]); ++it) {
			if (it->x > max.x)
				max.x = it->x;
		}

		if (_offsideLine.instances.at(0).p1.x < 0)
		{
			SetModelmat(min.x, fieldWidth);
		}

		if (_offsideLine.instances.at(0).p1.x > 0)
		{
			SetModelmat(max.x, fieldWidth);
		}
	}
}

/**
* @brief Function draws offside line instance
*/
void OffsideLine::Draw()
{
	_offsideLine.Draw();
}