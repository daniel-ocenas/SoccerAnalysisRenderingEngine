#include "PerspectiveCamera.hpp"



PerspectiveCamera::PerspectiveCamera(float fovy,
    float aspect,
    float near,
    float far)
{
    _fovy = fovy;
    _aspect = aspect;
    _near = near;
    _far = far;
    _recomputeView = true;
}

glm::mat4 PerspectiveCamera::getProjMatrix()
{
    if (_recomputeProj) {
        if (_far == std::numeric_limits<float>::infinity())
            _projMat = glm::infinitePerspective(glm::radians(_fovy), _aspect, _near);
        else
            _projMat = glm::perspective(glm::radians(_fovy), _aspect, _near, _far);
        _recomputeProj = false;
    }
    return _projMat;
}

glm::mat4 PerspectiveCamera::getViewMatrix()
{
    if (_recomputeView) {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.f), glm::radians(_xAngle), glm::vec3(1.f, 0.f, 0.f)) *
            glm::rotate(glm::mat4(1.f), glm::radians(_yAngle), glm::vec3(0.f, 1.f, 0.f));
        
        _viewMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -_distance)) *
            rotation * glm::translate(glm::mat4(1.f), _position);

        _recomputeView = false;
    }

    return _viewMat;
}

void PerspectiveCamera::setViewMat(glm::mat4 viewMatrix)
{
    _viewMat = viewMatrix;
}
void PerspectiveCamera::setRotMat(glm::mat4 rotMatrix)
{
    _rotMat = rotMatrix;
}

void PerspectiveCamera::setNear(float value) 
{
    _near = value;
    _recomputeProj = true;
}
void PerspectiveCamera::setFar(float value) 
{
    _far = value;
    _recomputeProj = true;
}
void PerspectiveCamera::setFovy(float value) 
{
    _fovy = value;
    _recomputeProj = true;
}
void PerspectiveCamera::setAspect(float value) 
{
    _aspect = value;
    _recomputeProj = true;
}

void PerspectiveCamera::setDistance(float value)
{
    _distance = value;
    _recomputeView = true;
}

void PerspectiveCamera::addDistance(float value)
{
    _distance += value;
    _recomputeView = true;
}

void PerspectiveCamera::addXPosition(float dx) 
{
    _position.x += dx;
    _recomputeView = true;
}

void PerspectiveCamera::addYPosition(float dy) 
{
    _position.y += dy;

    _recomputeView = true;
}
void PerspectiveCamera::addZPosition(float dz)
{
    _position.z += dz;
    _recomputeView = true;
}

void PerspectiveCamera::addXRotation(float dx)
{
    _xAngle += dx;
    if (_xAngle < 10)
        _xAngle = 10;
    if (_xAngle > 170)
        _xAngle = 170;
    _recomputeView = true;
}

void PerspectiveCamera::addYRotation(float dy)
{
    _yAngle += dy;
    _recomputeView = true;
}


void PerspectiveCamera::setXRotation(float dx)
{
    _xAngle = dx;
    _recomputeView = true;
}

void PerspectiveCamera::setYRotation(float dy)
{
    _yAngle = dy;
    _recomputeView = true;
}

void PerspectiveCamera::setPostion(glm::vec3 const& value)
{
    _position = value;
    _recomputeView = true;
}


void PerspectiveCamera::rotateXView(float value)
{
    float theta = glm::degrees(atan2(sqrt(_videoViewMat[2][1] * _videoViewMat[2][1] + _videoViewMat[2][0] * _videoViewMat[2][0]), _videoViewMat[2][2]));
    std::cout << "Theta" << theta << std::endl;
    if ((theta > 10 && theta < 160) || (value > 0 && theta > 0 && theta < 10) || (value < 0 && theta < 180 && theta > 160))
        rotateXMat(_videoViewMat, value);

}


void PerspectiveCamera::rotateYView(float value)
{
    rotateYMat(_videoViewMat, value);
}


void PerspectiveCamera::rotateZView(float value)
{
    rotateZMat(_videoViewMat, value);
}


void PerspectiveCamera::addViewDistance(float value)
{
    addDistanceMat(_videoViewMat, value);
}


void PerspectiveCamera::addXposView(float value)
{
    addXposMat(_videoViewMat, value);
}

void PerspectiveCamera::addYposView(float value)
{
    addYposMat(_videoViewMat, value);
}



void rotateXMat(glm::mat4 &mat, float value)
{
    mat = glm::rotate(mat, glm::radians(value), glm::vec3(1.0f, 0.0f, 0.0f));
}

void rotateYMat(glm::mat4 &mat, float value)
{
    mat = glm::rotate(mat, glm::radians(value), glm::vec3(0.0f, 1.0f, 0.0f));
}

void rotateZMat(glm::mat4 &mat, float value)
{
    mat = glm::rotate(mat, glm::radians(value), glm::vec3(0.0f, 0.0f, 1.0f));
}

void addDistanceMat(glm::mat4 &mat, float value)
{
    mat[3][2] += value;
}

void addXposMat(glm::mat4 &mat, float value)
{
    mat = glm::translate(mat, glm::vec3(value, 0.f, 0.f));
}

void addYposMat(glm::mat4 &mat, float value)
{
    mat = glm::translate(mat, glm::vec3(0.f, value, 0.f));
}

