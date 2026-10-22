#pragma once
#include <vector>
#include <glm.hpp>
#include<string>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include "Mesh_tn.h"
#include "Texture.h"

class Model
{
public:
	Model();

	bool LoadModel(const std::string& fileName);
	void RenderModel(GLint colorLocation, GLint textureLocation, GLint specularLocation, GLint shininessLocation);
	void ClearModel();

	~Model();

private:
	void LoadNode(aiNode* node, const aiScene* scene); //assimp
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	bool LoadMaterials(const aiScene* scene);
	std::vector<MeshModel*>MeshList;
	std::vector<Texture*>TextureList;
	std::vector<unsigned int>meshTotex;
    std::vector<glm::vec3> materialColors;
    std::vector<float> materialSpecular, materialShininess;
};


