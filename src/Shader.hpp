
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "Materials.hpp"


class Shader
{
public:
	Shader();
	~Shader();

	void compileShaders(std::string vname, std::string fname);
	GLuint getShaderProg();
	void use();
	int getULocation(std::string name);
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3f(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w);
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	void SetModelMatUniform(glm::mat4 modelMat);
	void SetMaterialUniforms(Material material);

private:
	void loadShaders();
	bool checkShaderCompiled(GLint shader);


	GLuint _vertexShaderID, _fragmentShaderID;
	GLuint _vertexID, _fragmentID;
	GLuint _shaderProgram;
	std::string _sPVertex, _sPFragment;
	std::string _vertexCode, _fragmentCode;
};

