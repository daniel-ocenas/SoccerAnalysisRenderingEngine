#pragma once

#include <iostream>

#include <fstream>
#include <sstream>
#include <vector> 
#include <algorithm>
#include <cstdlib>


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.hpp"
#include "common.hpp"


class ModelLoader
{
public:

	ModelLoader();

	void loadObjFile(std::string _fileName,
		GLuint& vao, 
		Mesh & meshObj);

	void loadObjFile(
		GLuint& vao,
		Mesh& meshObj,
		const Vertex vertices[],
		const UV uvs[],
		const Normal normals[],
		const int size
	);

	void initialzeModel(GLuint& vao,
		Mesh& meshObj);

};

