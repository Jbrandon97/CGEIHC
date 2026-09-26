#pragma once
#include <vector>
#include<string>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include <glm.hpp>

#include "Mesh_tn.h"

class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel(GLuint colorLocation, const glm::vec3& fallbackColor);
	void ClearModel();

	~Model();

private:
	void LoadNode(aiNode* node, const aiScene* scene); //assimp
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<MeshModel*>MeshList;
	std::vector<glm::vec3> meshColors;
	std::vector<bool> meshUsesMaterialColor;
};

