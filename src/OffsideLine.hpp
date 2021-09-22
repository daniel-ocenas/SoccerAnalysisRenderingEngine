#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GraphicObject.hpp"
#include "ModelLoader.hpp"
#include "Materials.hpp"
#include "Mesh.hpp"


//#include "objects3D/plane.hpp"

class OffsideLine : public GraphicObject
{
public:
	OffsideLine(std::string path);
	~OffsideLine();

	void Draw();
	void SetModelmat(float posX, float width);

	bool GetAutoOffside() { return _offsideLine.instances.at(0).animate; };
	void SetAutoOffside(bool value) { _offsideLine.instances.at(0).animate = value; };


	bool IsActive() { return _offsideLine.instances.at(0).active; };
	void SetActive(bool value) { _offsideLine.instances.at(0).active = value; };
	void InvertActive() { _offsideLine.instances.at(0).active = !_offsideLine.instances.at(0).active; };
	void AutoOffside(std::vector<std::vector<glm::vec2>> playerPositions, float fieldWidth);
	glm::mat4 GetModelMat() { return _offsideLine.instances.at(0).modelMat; }
	Material GetMaterial() { return _offsideLine.instances.at(0).material; }


private:

	GraphicObject _offsideLine;
	float posX = 0;
	glm::mat4 modelMat = glm::mat4(1.f);
	Material material = REDPLASTIC;
};

