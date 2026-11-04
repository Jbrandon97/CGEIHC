#include "Model.h"
#include <unordered_map>
#include <unordered_set>


Model::Model()
{
}

bool Model::LoadModel(const std::string & fileName)
{
	Assimp::Importer importer;//					Pasa de Polygons y Quads a triangulos, modifica orden para el origen, generar normales si el  objeto no tiene, trata vÃƒÂ©rtices iguales como 1 solo
	//const aiScene *scene=importer.ReadFile(fileName,aiProcess_Triangulate |aiProcess_FlipUVs|aiProcess_GenSmoothNormals|aiProcess_JoinIdenticalVertices);
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!scene)
	{	
		printf("Fallo en cargar el modelo: %s (%s)\n", fileName.c_str(), importer.GetErrorString());
		return false;
	}
	LoadNode(scene->mRootNode, scene);
	bool ok = LoadMaterials(scene);
	printf("Modelo %s: %zu mallas, texturas %s\n", fileName.c_str(), MeshList.size(), ok ? "OK" : "ERROR");
	return ok && !MeshList.empty();
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

	std::unordered_set<Texture*> released;
	for (unsigned int i = 0; i < TextureList.size(); i++)
	{
		if (TextureList[i])
		{
			if (released.insert(TextureList[i]).second) delete TextureList[i];
			TextureList[i] = nullptr;
		}
	}

	MeshList.clear();
	TextureList.clear();
	meshTotex.clear();
    materialColors.clear(); materialSpecular.clear(); materialShininess.clear();
}

void Model::RenderModel(GLint colorLocation, GLint textureLocation, GLint specularLocation, GLint shininessLocation)
{
    for (size_t i = 0; i < MeshList.size(); ++i) {
        unsigned int m = meshTotex[i];
        glUniform3fv(colorLocation, 1, &materialColors[m][0]);
        glUniform1i(textureLocation, TextureList[m] != nullptr);
        glUniform1f(specularLocation, materialSpecular[m]);
        glUniform1f(shininessLocation, materialShininess[m]);
        if (TextureList[m]) TextureList[m]->UseTexture();
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
		// Conservar las normales exteriores del modelo.
		
		vertices.insert(vertices.end(), { mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z });
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	MeshModel* newMesh = new MeshModel();
	newMesh->CreateMeshModel(&vertices[0], &indices[0], static_cast<unsigned int>(vertices.size()), static_cast<unsigned int>(indices.size()));
	MeshList.push_back(newMesh);
	meshTotex.push_back(mesh->mMaterialIndex);
}

bool Model::LoadMaterials(const aiScene* scene)
{
    bool ok = true;
    TextureList.resize(scene->mNumMaterials, nullptr);
    materialColors.resize(scene->mNumMaterials, glm::vec3(1));
    materialSpecular.resize(scene->mNumMaterials, .2f);
    materialShininess.resize(scene->mNumMaterials, 16.0f);
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* material = scene->mMaterials[i];
        aiColor3D diffuse(1,1,1), specular(.2f,.2f,.2f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material->Get(AI_MATKEY_SHININESS, materialShininess[i]);
        materialColors[i] = glm::vec3(diffuse.r,diffuse.g,diffuse.b);
        materialSpecular[i] = (specular.r+specular.g+specular.b)/3;
        if (materialShininess[i] < 1) materialShininess[i] = 1;
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE,0,&path)==AI_SUCCESS) {
            std::string filename(path.C_Str());
            auto separator=filename.find_last_of("/\\");
            if(separator!=std::string::npos)filename=filename.substr(separator+1);
            std::string full="Textures/"+filename;
            TextureList[i]=new Texture(full.c_str());
            if(!TextureList[i]->LoadTextureA())ok=false;
        }
        // Sin mapa difuso, usar Kd del MTL directamente, nunca plain.png.
    }
    return ok;
}
