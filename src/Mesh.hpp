#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

struct Mesh {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texture_uv;
	std::vector<unsigned int> indices;

	GLuint textureID;
};

