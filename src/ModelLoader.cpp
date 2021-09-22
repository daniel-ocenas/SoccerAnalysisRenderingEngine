#include "ModelLoader.hpp"

ModelLoader::ModelLoader()
{
}
void ModelLoader::loadObjFile(
	GLuint& vao,
	Mesh& meshObj,
	const Vertex vertices[],
	const UV uvs[],
	const Normal normals[],
	int size
	)
{

	for (int i = 0; i < size; i++)
	{
		meshObj.vertices.push_back(glm::vec3(vertices[i].position[0], vertices[i].position[1], vertices[i].position[2]));
		meshObj.uvs.push_back(glm::vec2(uvs[i].position[0], uvs[i].position[1]));
		meshObj.normals.push_back(glm::vec3(normals[i].position[0], normals[i].position[1], normals[i].position[2]));
	}

	initialzeModel(vao, meshObj);
}

std::string printNum(float number)
{

	return std::to_string(number);
}

void ModelLoader::loadObjFile(std::string fileName, 
	GLuint& vao, 
	Mesh &meshObj )
{
	// Check the file name to make sure it has the OBJ extension
	std::string ext = fileName;
	for (int i = 0; i < ext.length(); i++)
	{
		ext.at(i) = tolower(ext.at(i));
	}
	if (ext.substr(ext.find_last_of(".") + 1) != "obj")
	{
		throw("Filename is not a valid .obj file.\nPlease change your settings and try again.");
	};
	Assimp::Importer import;

	const aiScene* scene = import.ReadFile(fileName, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error("Invalid object file!\n");
	}

	//for extension (support for more meshes)
	unsigned num_meshes = 1; // scene->mNumMeshes;
	for (unsigned i = 0; i < num_meshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];

		for (unsigned j = 0; j < mesh->mNumVertices; ++j)
		{
			aiVector3D pos = mesh->mVertices[j];
			//setting vertex coords
			meshObj.vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));

			//support for 1 set UV coords
			if (mesh->mTextureCoords[0] != nullptr)
			{
				//setting uv coords
				aiVector3D uv = mesh->mTextureCoords[0][j];
				meshObj.uvs.push_back(glm::vec2(uv.x, uv.y));
			}

			//setting normals
			if (mesh->mNormals != nullptr)
			{
				aiVector3D normal = mesh->mNormals[j];
				meshObj.normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
			}
		}
		//setting indicies
		if (mesh->HasFaces())
		{
			for (unsigned j = 0; j < mesh->mNumFaces; ++j)
			{
				aiFace face = mesh->mFaces[j];
				for (unsigned l = 0; l < face.mNumIndices; ++l)
				{
					meshObj.indices.push_back(face.mIndices[l]);
				}
			}
		}
	}

	//std::cout << "____________________________________" << std::endl;
	//std::cout << "____________________________________" << std::endl;
	//std::cout << "____________________________________" << std::endl;
	//std::cout << "____________________________________" << std::endl;
	//std::cout << "____________________________________" << std::endl;
	//std::cout << "____________________________________" << std::endl;
	//std::cout << "____________________________________" << std::endl;
	//std::cout << "____________________________________" << std::endl;
	//for (int i = 0; i < meshObj.vertices.size(); i++)
	//{
	//	std::cout << "{ " << printNum(meshObj.vertices[i].x) << "f," << printNum(meshObj.vertices[i].y) << "f," << printNum(meshObj.vertices[i].z) << "f}," << std::endl;
	//}

	//std::cout << "*************************************" << std::endl;
	//std::cout << "*************************************" << std::endl;
	//std::cout << "*************************************" << std::endl;
	//std::cout << "*************************************" << std::endl;
	//std::cout << "*************************************" << std::endl;
	//std::cout << "*************************************" << std::endl;
	//std::cout << "*************************************" << std::endl;
	//for (int i = 0; i < meshObj.uvs.size(); i++)
	//{
	//	std::cout << "{ " << printNum(meshObj.uvs[i].x) << "f," << printNum(meshObj.uvs[i].y) << "f}," << std::endl;
	//}

	//std::cout << "#####################################" << std::endl;
	//std::cout << "#####################################" << std::endl;
	//std::cout << "#####################################" << std::endl;
	//std::cout << "#####################################" << std::endl;
	//std::cout << "#####################################" << std::endl;
	//std::cout << "#####################################" << std::endl;
	//std::cout << "#####################################" << std::endl;
	//for (int i = 0; i < meshObj.normals.size(); i++)
	//{
	//	std::cout << "{ " << printNum(meshObj.normals[i].x) << "f," << printNum(meshObj.normals[i].y) << "f," << printNum(meshObj.normals[i].z) << "f}," << std::endl;
	//}


	initialzeModel(vao, meshObj);

}



void ModelLoader::initialzeModel(GLuint &vao,
	Mesh& meshObj)
{
	vao = 0;
	// Creates one VAO for our object
	glGenVertexArrays(1, &vao);

	// Bind out VAO in memory and "turning it on"
	glBindVertexArray(vao);

	// Vertex buffer
	GLuint bufferV = 0;
	glGenBuffers(1, &bufferV);
	glBindBuffer(GL_ARRAY_BUFFER, bufferV);
	// Assign the data with the size of our vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshObj.vertices.size(), &meshObj.vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		0,                  // stride
		0					// array buffer offset
	);
	// Activate it
	glEnableVertexAttribArray(0);

	// UV
	if (meshObj.uvs.size() > 0)
	{
		// Same principle as our vertices but don't activate unless we actually have loaded UVs
		// OpenGL will error if you don't have any data to buffer
		// Our attribute this time is 1 which changes our layout input location in the shader
		GLuint bufferU = 0;
		glGenBuffers(1, &bufferU);
		glBindBuffer(GL_ARRAY_BUFFER, bufferU);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * meshObj.uvs.size(), &meshObj.uvs.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                  // attribute
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			0            // array buffer offset
		);
		glEnableVertexAttribArray(1);
	}


	// Normals
	if (meshObj.normals.size() > 0)
	{
		// Same once again
		// Attribute this time is 2
		GLuint bufferN = 0;
		glGenBuffers(1, &bufferN);
		glBindBuffer(GL_ARRAY_BUFFER, bufferN);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshObj.normals.size(), &meshObj.normals.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(
			2,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			0            // array buffer offset
		);
		glEnableVertexAttribArray(2);
	}

	// Bind our array buffer and vertex array and send them into GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Disable all our cached vertex arrays
	glDisableVertexAttribArray(0);
	if (meshObj.uvs.size() > 0)
	{
		glDisableVertexAttribArray(1);
	}
	if (meshObj.normals.size() > 0)
	{
		glDisableVertexAttribArray(2);
	}
}

