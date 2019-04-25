#pragma once

#include "DDSTextureLoader.h"
#include <vector>

#include "D3D.h"
#include "Model.h"

//Stores Models and Textures to be used in the application
class ResourceManager
{
public:
	enum ManagerScene
	{
		WINTER_REFLECTION,
		PROCEDURAL,
		SCENES_MAX
	};

	enum TextureResource
	{
		ORBIT_TEXTURE,
		SKY_CUBE_TEXTURE,
		TREE_TEXTURE,
		TERRAIN_TEXTURE,
		TEXTURES_MAX
	};

	enum ModelResource
	{
		ORBIT_MODEL,
		SKY_DOME_MODEL,
		REFLECTIVE_MODEL,
		MODELS_MAX
	};

public:

	ResourceManager(ID3D11Device*, ID3D11DeviceContext*, ManagerScene);
	~ResourceManager();

	bool LoadResources();
	ID3D11ShaderResourceView** GetTextures(TextureResource);
	Model* GetModel(ModelResource);

private:
	bool LoadTexture(TextureResource);
	bool LoadModel(ModelResource);

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	std::vector<ID3D11ShaderResourceView**> m_textures;
	int m_textureCount;
	std::vector<Model*> m_models;

	ManagerScene m_scene;

};