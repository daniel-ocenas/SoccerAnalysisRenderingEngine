#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>


#include "common.hpp"
#include "Materials.hpp"
#include "Mesh.hpp"

class GraphicObject
{
public:
	GraphicObject();
	~GraphicObject();

	virtual void Draw();

	void CreateObjectInstance(glm::mat4 modelMat, Material material);
	void DeleteObjectInstance(int instance);
	void SetObjectMat(glm::mat4 modelMat, int instance);
	void SetMaterial(int colorId, int instance);
	glm::mat4 GetObjectMat(int instance);

	std::vector<glm::vec3> getVertices() { return mesh.vertices;	};
	std::vector<glm::vec2> getUvs() { return mesh.uvs;	};
	std::vector<glm::vec3> getNormals() { return mesh.normals; };
	GLuint getTextureID() { return mesh.textureID; };
	Mesh getMesh() { return mesh; };
	GLuint getVao() { return vao; };

	int getVerticesCount() { return (int)mesh.vertices.size(); };
	int getUVCount() { return (int)mesh.uvs.size(); };
	int getNormalCount() { return (int)mesh.normals.size(); };

	size_t getVerticesByte() const 	{ return mesh.vertices.size() * 3 * sizeof(float); }
	size_t getNormalsByte() const { return mesh.normals.size() * 3 * sizeof(float); }
	size_t getTextureUvByte() const	{ return mesh.uvs.size() * 2 * sizeof(float); }
	size_t getIndicesByte() const {	return mesh.indices.size() * sizeof(unsigned int); }

	struct ObjectInstance {
		glm::mat4 modelMat;
		Material material;
		int color = 0;
		bool active = true;
		bool animate = false;
		glm::ivec2 p1;
		glm::ivec2 p2;
	};

	std::vector<ObjectInstance> instances;
	Mesh mesh;
	GLuint vao;

};

