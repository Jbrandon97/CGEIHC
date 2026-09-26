#include "Model.h"
#include <iostream>



Model::Model()
{
}

void Model::LoadModel(const std::string & fileName)
{
	Assimp::Importer importer;//					Pasa de Polygons y Quads a triangulos, modifica orden para el origen, generar normales si el  objeto no tiene, trata v�rtices iguales como 1 solo
	//const aiScene *scene=importer.ReadFile(fileName,aiProcess_Triangulate |aiProcess_FlipUVs|aiProcess_GenSmoothNormals|aiProcess_JoinIdenticalVertices);
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!scene)
	{	
		printf("No se pudo cargar %s: %s\n", fileName.c_str(), importer.GetErrorString());
		return;
	}
	LoadNode(scene->mRootNode, scene);
	}

void Model::ClearModel()
{
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		if (MeshList[i])
		{
			delete MeshList[i];
			MeshList[i] = nullptr;

		}
	}
	MeshList.clear();
	meshColors.clear();
	meshUsesMaterialColor.clear();
}

void Model::RenderModel(GLuint colorLocation, const glm::vec3& fallbackColor)
{
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		// Conserva el color del MTL cuando existe; si no, usa el color indicado por el modelo.
		const glm::vec3& color = meshUsesMaterialColor[i] ? meshColors[i] : fallbackColor;
		glUniform3fv(colorLocation, 1, &color[0]);
		MeshList[i]->RenderMeshModel();

	}


}


Model::~Model()
{
}

void Model::LoadNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i <node->mNumMeshes; i++)
	{
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

void Model::LoadMesh(aiMesh * mesh, const aiScene * scene)
{

	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x,mesh->mVertices[i].y ,mesh->mVertices[i].z });
		//UV
		if (mesh->mTextureCoords[0])//si tiene coordenadas de texturizado
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y});
		}
		else
		{
			vertices.insert(vertices.end(), { 0.0f,0.0f });
		}
		//Normals importante, las normales son negativas porque la luz interact�a con ellas de esa forma, c�mo se vio con el 
		
		vertices.insert(vertices.end(), { -mesh->mNormals[i].x,-mesh->mNormals[i].y ,-mesh->mNormals[i].z });
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	MeshModel* newMeshModel = new MeshModel();
	newMeshModel->CreateMeshModel(&vertices[0], &indices[0],
		static_cast<unsigned int>(vertices.size()),
		static_cast<unsigned int>(indices.size()));
	MeshList.push_back(newMeshModel);

	// Assimp separa los grupos de material del OBJ en distintas mallas.
	// Se guarda el color difuso para dibujar cada parte con su apariencia original.
	glm::vec3 materialColor(0.0f);
	bool hasMaterialColor = false;
	if (mesh->mMaterialIndex < scene->mNumMaterials)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString materialName;
		aiColor3D diffuseColor;
		material->Get(AI_MATKEY_NAME, materialName);
		if (std::string(materialName.C_Str()) != "DefaultMaterial" &&
			material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
		{
			materialColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
			hasMaterialColor = true;
		}
	}
	meshColors.push_back(materialColor);
	meshUsesMaterialColor.push_back(hasMaterialColor);
}

