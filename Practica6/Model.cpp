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
}

void Model::RenderModel()
{
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		unsigned int materialIndex = meshTotex[i];
		if (materialIndex < TextureList.size() && TextureList[materialIndex])
		{
			TextureList[materialIndex]->UseTexture();
		}
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

bool Model::LoadMaterials(const aiScene * scene)
{
	bool ok = true;
	std::unordered_map<std::string, Texture*> loadedTextures; // Mapa para evitar duplicados
	TextureList.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene ->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		TextureList[i] = nullptr;
		if (material->GetTextureCount(aiTextureType_DIFFUSE	))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE,0,&path)==AI_SUCCESS)
			{
				// Aceptar separadores de Windows y de Blender; las imagenes viven en Textures.
				std::string filename(path.C_Str());
				const auto separator = filename.find_last_of("/\\");
				if (separator != std::string::npos) filename = filename.substr(separator + 1);
				std::string tga ="tga";
				std::string png = "png";
				std::size_t existetga = filename.find(tga);
				std::size_t existepng= filename.find(png);
				std::string texPath = std::string("Textures/") + filename;
				// Verifica si ya se cargÃƒÂ³ esta textura antes
				auto it = loadedTextures.find(texPath);
				if (it != loadedTextures.end())
				{
					TextureList[i] = it->second; // Reutiliza la textura ya cargada
				}
				else
				{
					Texture* newTex = new Texture(texPath.c_str());
					std::string ext = filename.substr(filename.find_last_of('.') + 1);

					bool loaded = false;
					if (ext == "tga" || ext=="png")
					{
						loaded = newTex->LoadTextureA();
					}
					else
					{
						loaded = newTex->LoadTexture();
					}

					if (loaded)
					{
						TextureList[i] = newTex;
						loadedTextures[texPath] = newTex; // Almacena para reutilizarla
					}
					else
					{
						printf("FallÃƒÂ³ en cargar la Textura :%s\n", texPath.c_str());
						delete newTex;
                        ok = false;
					}
				}

			/*	TextureList[i] = new Texture(texPath.c_str());
				if (existetga != std::string::npos || existepng != std::string::npos)
				{
					if (!TextureList[i]->LoadTextureA())
					{
						printf("FallÃƒÂ³ en cargar la Textura :%s\n", texPath);
						delete TextureList[i];
						TextureList[i] = nullptr;
					}
				}
				else
				{
					if (!TextureList[i]->LoadTexture())
					{
						printf("FallÃƒÂ³ en cargar la Textura :%s\n", texPath);
						delete TextureList[i];
						TextureList[i] = nullptr;
					}
				}*/
			}
		}
		//Si no se pudo cargar la textura o no tiene una textura asignada, asignar una textura por defecto
		if (!TextureList[i])
		{
			TextureList[i] = new Texture("Textures/plain.png"); //textura que se aplicarÃƒÂ¡ a los modelos si no tienen textura o la textura no se puede cargar
			if (!TextureList[i]->LoadTextureA()) ok = false;
		}

	}
    return ok;
}
