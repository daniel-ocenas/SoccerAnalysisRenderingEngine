#include "GraphicObject.hpp"

GraphicObject::GraphicObject()
{
}


GraphicObject::~GraphicObject()
{
    glDeleteVertexArrays(1, &vao);
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.uvs.clear();
}


void GraphicObject::CreateObjectInstance(glm::mat4 modelMat, Material material)
{
    instances.push_back(ObjectInstance{ modelMat , material });
}

void GraphicObject::DeleteObjectInstance(int instance)
{
    if (instance < instances.size())
    {
        instances.erase(instances.begin() + instance);
    }
}


void GraphicObject::SetObjectMat(glm::mat4 modelMat, int instance)
{
    if (instance < instances.size())
    {
        instances.at(instance).modelMat = modelMat;
    }
}


void GraphicObject::SetMaterial(int colorId, int instance)
{
    if (instance < instances.size())
    {
        instances.at(instance).material = colors[colorId].material;
        instances.at(instance).color = colorId;
    }
}

glm::mat4 GraphicObject::GetObjectMat(int instance)
{
    return instances.at(instance).modelMat;
}

void GraphicObject::Draw()
{
	// Activate the VAO
	glBindVertexArray(vao);

	// Triangles are drawn based on the size of our vertices
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei) mesh.vertices.size());

	// Unbind VAO
	glBindVertexArray(0);	

}