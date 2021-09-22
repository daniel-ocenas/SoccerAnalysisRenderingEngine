#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>


void rotateXMat(glm::mat4 &mat, float value);
void rotateYMat(glm::mat4 &mat, float value);
void rotateZMat(glm::mat4 &mat, float value);

void addXposMat(glm::mat4 &mat, float value);
void addYposMat(glm::mat4 &mat, float value);
void addDistanceMat(glm::mat4 &mat, float value);


class PerspectiveCamera
{
public:
	PerspectiveCamera(float fovy, float aspect, float near, float far);
	glm::mat4 getProjMatrix();
	glm::mat4 getViewMatrix();
	glm::mat4 getVideoViewMatrix() { return _videoViewMat; };

	float getNear() const { return _near; }
	float getFar() const { return _far; }
	float getFovy() const { return _fovy; }
	float getAspect() const { return _aspect; }
	float getDistance() const { return _distance; }


	void setNear(float value);
	void setFar(float value);
	void setFovy(float value);
	void setAspect(float value);
	
	void setViewMat(glm::mat4 viewMatrix);
	void setRotMat(glm::mat4 rotMatrix);

	void setDistance(float value);
	void addDistance(float value);
	
	void setPostion(glm::vec3 const& value);
	void addXPosition(float dx);
	void addYPosition(float dy);
	void addZPosition(float dz);

	void addXRotation(float dx);
	void addYRotation(float dy);
	void setXRotation(float dx);
	void setYRotation(float dy);


	//video view mat funcs
	void setVideoViewMatrix(glm::mat4 videoViewMat) { _videoViewMat = videoViewMat; }
	void addViewDistance(float value);
	void rotateXView(float value);
	void rotateYView(float value);
	void rotateZView(float value);

	void addXposView(float value);
	void addYposView(float value);

	void SetExternalParams(float dd, float dxRot, float dyRot, glm::vec3 pos)
	{
		_distance = dd;
		_xAngle = dxRot;
		_yAngle = dyRot;
		_position = pos;
		_recomputeView = true;
	}


private:
	glm::mat4 _projMat = glm::mat4(1.f);
	glm::mat4 _viewMat = glm::mat4(1.f);
	glm::mat4 _videoViewMat = glm::mat4(1.f);
	glm::vec3 _position = glm::vec3(0.f, 0.f, 0.f);

	glm::mat4 _rotMat = glm::mat4(1.f);

	float   _near = 1.f;
	float   _far = 1000.f;
	float   _fovy = 1.f;
	float	_aspect = 45.f;


	float	_distance = 1.f;
	float	_xAngle = 0.f;
	float	_yAngle = 0.f;
	bool    _recomputeView = true;
	bool    _recomputeProj = true;

};


