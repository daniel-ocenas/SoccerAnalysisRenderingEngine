#include "Arrow.hpp"

/**
* @brief Arrow Constructor
*/
Arrow::Arrow(std::string path)
{
	cylinder = std::make_unique<GraphicObject>();
	tube = std::make_unique<GraphicObject>();
    ModelLoader loader;
	//loader.loadObjFile(cylinder->vao, cylinder->mesh, cylinderVertices, cylinderUVs, cylinderNormals, cylinderSize);
	//loader.loadObjFile(tube->vao, tube->mesh, tubeVertices, tubeUVs, tubeNormals, tubeSize);
	loader.loadObjFile(path + "\\graphics\\objects3D\\cylinder.obj", cylinder->vao, cylinder->mesh);
	loader.loadObjFile(path + "\\graphics\\objects3D\\tube.obj", tube->vao, tube->mesh);
}


/**
* @brief Deletes arrow instance matrices
*/
void Arrow::DeleteArrowInstance(int instance)
{
	if (instance < cylinder->instances.size() && instance < tube->instances.size())
	{
		cylinder->instances.erase(cylinder->instances.begin() + instance);
		tube->instances.erase(tube->instances.begin() + instance);
		instancesCnt--;
	}
}

bool Arrow::GetInstanceAnimate(int instance)
{
	if (instance < cylinder->instances.size())
	{
		return cylinder->instances.at(instance).animate;
	}
	return false;
}

void Arrow::SetInstanceAnimate(int instance, bool value)
{
	if (instance < cylinder->instances.size() && instance < tube->instances.size())
	{

		cylinder->instances.at(instance).animate = value;
		tube->instances.at(instance).animate = value;
		if (!value)
		{
			glm::vec4 points = GetArrowPoints(instance);
			SetDisplayParameters(glm::vec2(points.x, points.y), glm::vec2(points.z, points.w), instance);
		}
	}
}


bool Arrow::IsActive(int instance)
{
	if (instance < cylinder->instances.size())
	{
		return cylinder->instances.at(instance).active;
	}
	return false;
}

void Arrow::SetActive(bool value, int instance)
{
	if (instance < cylinder->instances.size() && instance < tube->instances.size())
	{

		cylinder->instances.at(instance).active = value;
		tube->instances.at(instance).active = value;
	}
}

void Arrow::TranslateArrowInstancce(glm::vec3 translation, int instance)
{
	if (instance < cylinder->instances.size() && instance < tube->instances.size())
	{
		SetArrowMats(
			glm::translate(glm::mat4(cylinder->instances.at(instance).modelMat), translation),
			glm::translate(glm::mat4(tube->instances.at(instance).modelMat), translation), 
			instance
		);
	}
}


/**
* @brief Arrow material set
*/
void Arrow::SetMaterial(int colorId, int instance)
{
	if (instance < cylinder->instances.size())
		cylinder->SetMaterial(colorId, instance);

	if (instance < tube->instances.size())
		tube->SetMaterial(colorId, instance);
}

int Arrow::GetInstanceColorId(int instance)
{
	return cylinder->instances.at(instance).color;
}

/**
* @brief Arrow model matrix set
*/
void Arrow::SetArrowMats(glm::mat4 cylinderModelMat, glm::mat4 tubeModelMat, int instance)
{
	if (instance < cylinder->instances.size())
	{
		cylinder->instances.at(instance).modelMat = cylinderModelMat;
	}

	if (instance < tube->instances.size())
	{
		tube->instances.at(instance).modelMat = tubeModelMat;
	}
}

/**
* @brief Creates arrow instance matrices
*/
void Arrow::CreateArrowInstance(glm::mat4 modelMat, Material material, int ColorId)
{
	cylinder->instances.push_back(ObjectInstance{ modelMat , material, ColorId });
	tube->instances.push_back(ObjectInstance{ modelMat , material, ColorId });
	instancesCnt++;
}

/**
* @brief Creates arrow instance with initial points
*/
void Arrow::CreateArrowInstance(glm::vec2 p1, glm::vec2 p2, int ColorId)
{
	cylinder->instances.push_back(ObjectInstance{ glm::mat4(0.f) , colors[ColorId].material,ColorId });
	tube->instances.push_back(ObjectInstance{ glm::mat4(0.f) ,  colors[ColorId].material,ColorId });
	SetDisplayParameters(p1, p2, instancesCnt);
	instancesCnt++;
}

/**
* @brief Function returns object points in single glm::vec4
*/
glm::vec4 Arrow::GetArrowPoints(int instance)
{
	if (instance < cylinder->instances.size())
	{
		glm::vec4 arrowPoints;
		arrowPoints.x = (float) cylinder->instances.at(instance).p1.x;
		arrowPoints.y = (float) cylinder->instances.at(instance).p1.y;
		arrowPoints.z = (float) cylinder->instances.at(instance).p2.x;
		arrowPoints.w = (float) cylinder->instances.at(instance).p2.y;
		return arrowPoints;
	}
	else {
		throw("Invalid Arrow instance indexing");
	}
}



/**
* @brief Generates arrow model matrix based on starting and end point
*/
void Arrow::SetDisplayParameters(glm::vec2 p1, glm::vec2 p2, int instance, float animationLength)
{
	float maxLength = (float)sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
	float multiplier = 0.f;
	if (maxLength > 1.5f && maxLength < 3.5f)
		multiplier = 0.5f;
	if (maxLength > 3.5f && maxLength < 7.f)
		multiplier = 1.1f;
	else if (maxLength >= 7.f && maxLength < 10.f)
		multiplier = 1.2f;
	else if (maxLength >= 10.f && maxLength < 25.f)
		multiplier = 1.3f;
	else if (maxLength >= 25.f && maxLength < 41.f)
		multiplier = 1.35f;
	else if (maxLength >= 41.f && maxLength < 100.f)
		multiplier = 1.4f;
	else if (maxLength >= 100.f )
		multiplier = 1.4f;
	float tubeLength = maxLength * multiplier * animationLength;
	float tubeScale = (tubeLength > 0.1f) ? scale : 0.f;


	glm::mat4 translateMatTube = glm::translate(glm::mat4(1.0f), glm::vec3(p1.x, worldHeight, p1.y));
	glm::mat4 scaleMatTube = glm::scale(glm::mat4(1.f), glm::vec3(tubeScale, tubeScale, tubeLength));


	float rotation = (float)-atan2(p2.y - p1.y, p2.x - p1.x);
	glm::mat4 rotationMat = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(.0f, 1.0f, .0f))
		* glm::rotate(glm::mat4(1.f), rotation, glm::vec3(.0f, 1.0f, .0f));

	//glm::vec2 cylinderPos = glm::vec2(
	//	p1.y + (tubeLength - scale) * -cos(rotation),
	//	p1.x + (tubeLength - scale) * -sin(rotation)
	//);

	glm::mat4 translateMatCylinder = glm::translate(glm::mat4(1.0f), glm::vec3(p2.x, worldHeight, p2.y));
	glm::mat4 scaleMatCylinder = glm::scale(glm::mat4(1.f), glm::vec3(scale));

	glm::mat4 finalMatCylinder =  translateMatCylinder * rotationMat *  scaleMatCylinder;
	glm::mat4 finalMatTube = translateMatTube * rotationMat *  scaleMatTube;

	SetArrowMats(finalMatCylinder, finalMatTube, instance);

}



void Arrow::SetPoints(glm::vec2 p1, glm::vec2 p2, int instance)
{
	if (instance < tube->instances.size() && instance < cylinder->instances.size())
	{
		cylinder->instances.at(instance).p1 = p1;
		cylinder->instances.at(instance).p2 = p2;
		tube->instances.at(instance).p1 = p1;
		tube->instances.at(instance).p2 = p2;
	}
}

/**
* @brief Arrow Draw function
*/
void Arrow::Draw(Shader* shader, int i, float time)
{
	if (i < tube->instances.size() && i < cylinder->instances.size())
	{
		if (i > higlightingArrowsOffset-1 && tube->instances.at(i).animate)
		{
			float animationLength = (float)0.5f * fmodf(time, 2.f);;
				SetDisplayParameters(cylinder->instances.at(i).p1, cylinder->instances.at(i).p2, i, animationLength);
		}



		shader->SetMaterialUniforms(tube->instances.at(i).material);
		shader->SetModelMatUniform(tube->instances.at(i).modelMat);

		glBindVertexArray(tube->vao);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)tube->mesh.vertices.size());
		glBindVertexArray(0);


		shader->SetMaterialUniforms(cylinder->instances.at(i).material);
		shader->SetModelMatUniform(cylinder->instances.at(i).modelMat);

		glEnable(GL_LINE_SMOOTH);
		glBindVertexArray(cylinder->vao);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cylinder->mesh.vertices.size());
		glBindVertexArray(0);
	}
}