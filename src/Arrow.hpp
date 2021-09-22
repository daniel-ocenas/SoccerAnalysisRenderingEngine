#pragma once
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "common.hpp"
#include "Shader.hpp"
#include "GraphicObject.hpp"
#include "ModelLoader.hpp"

class Arrow : public GraphicObject
{
public:
	Arrow(std::string path);

	void Draw(Shader* shader, int i, float time);
	bool IsActive(int instance);
	void SetActive(bool value, int instance);
	void SetPoints(glm::vec2 p1, glm::vec2 p2, int instance);
	
	void CreateArrowInstance(glm::mat4 modelMat, Material material, int ColorId);
	void CreateArrowInstance(glm::vec2 p1, glm::vec2 p2, int ColorId);

	void SetDisplayParameters(glm::vec2 p1, glm::vec2 p2, int instance, float animationLength = 1);

	void DeleteArrowInstance(int instance);
	void SetArrowMats(glm::mat4 cylinderModelMat, glm::mat4 tubeModelMat, int instance);
	void SetMaterial(int colorId, int instance);
	void TranslateArrowInstancce(glm::vec3 translation, int instance);
	int GetInstanceColorId(int instance);
	
	bool GetInstanceAnimate(int instance);
	void SetInstanceAnimate(int instance, bool value);


	glm::vec4 GetArrowPoints(int instance);

	int instancesCnt = 0;
	int higlightingArrowsOffset = 20;
private:

	std::unique_ptr<GraphicObject> cylinder;
	std::unique_ptr<GraphicObject> tube;

	float worldHeight = 1.f;
	float scale = 8.f;

};

