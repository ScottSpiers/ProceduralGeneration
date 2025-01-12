
#include  "ResourceManager.h"

ResourceManager::ResourceManager(ID3D11Device* device, ID3D11DeviceContext* context, ManagerScene scene)
{
	m_device = device;
	m_context = context;
	m_scene = scene;
	m_textureCount = 0;
}

ResourceManager::~ResourceManager()
{
	for (auto& m : m_models)
	{
		if (m)
		{
			delete m;
			m = 0;
		}
	}

	for (auto& t : m_textures)
	{
		int numElems = sizeof(t) / sizeof(t[0]);
		for (int i = 0; i < numElems; ++i)
		{
			if (t[i])
			{
				t[i]->Release();
				t[i] = 0;
			}
		}
	}
}

bool ResourceManager::LoadResources()
{
	bool result;

	switch (m_scene)
	{
		case WINTER_REFLECTION:
		{
			//Load the orbitting sphere texture and model
			if (!LoadTexture(ORBIT_TEXTURE))
			{
				return false;
			}

			if (!LoadModel(ORBIT_MODEL))
			{
				return false;
			}

			m_models[ORBIT_MODEL]->SetTextures(m_textures[ORBIT_TEXTURE]);

			//load the sky sphere texture and model
			if (!LoadTexture(SKY_CUBE_TEXTURE))
			{
				return false;
			}

			if (!LoadModel(SKY_DOME_MODEL))
			{
				return false;
			}

			m_models[SKY_DOME_MODEL]->SetTextures(m_textures[SKY_CUBE_TEXTURE]);

			// load the refelctive sphere model
			if (!LoadModel(REFLECTIVE_MODEL))
			{
				return false;
			}

			if (!m_models[ORBIT_MODEL]->InitializeBuffers(m_device))
			{
				return false;
			}

			if (!m_models[REFLECTIVE_MODEL]->InitializeBuffers(m_device))
			{
				return false;
			}

			if (!m_models[SKY_DOME_MODEL]->InitializeBuffers(m_device))
			{
				return false;
			}
			break;
		}
		case PROCEDURAL:
		{
			if (!LoadTexture(ORBIT_TEXTURE))
			{
				return false;
			}

			if (!LoadModel(ORBIT_MODEL))
			{
				return false;
			}

			m_models[ORBIT_MODEL]->SetTextures(m_textures[ORBIT_TEXTURE]);

			if (!m_models[ORBIT_MODEL]->InitializeBuffers(m_device))
			{
				return false;
			}

			if (!LoadTexture(SKY_CUBE_TEXTURE))
			{
				return false;
			}

			if(!LoadModel(SKY_DOME_MODEL))
			{
				return false;
			}

			m_models[SKY_DOME_MODEL]->SetTextures(m_textures[SKY_CUBE_TEXTURE]);

			if (!m_models[SKY_DOME_MODEL]->InitializeBuffers(m_device))
			{
				return false;
			}

			/*if (!LoadModel(TREE_MODEL))
			{
				return false;
			}*/

			if (!LoadTexture(TREE_TEXTURE))
			{
				return false;
			}

			/*m_models[TREE_MODEL]->SetTextures(m_textures[TREE_TEXTURE]);

			if (!m_models[TREE_MODEL]->InitializeBuffers(m_device))
			{
				return false;
			}*/

			if (!LoadTexture(TERRAIN_TEXTURE))
			{
				return false;
			}


			break;
		}
		default: return false;
	}

	return true;

}

ID3D11ShaderResourceView** ResourceManager::GetTextures(TextureResource t)
{
	return m_textures[t];
}

Model* ResourceManager::GetModel(ModelResource m)
{
	return m_models[m];
}

bool ResourceManager::LoadTexture(TextureResource t)
{
	HRESULT result;
	ID3D11ShaderResourceView* texture;
	ID3D11ShaderResourceView* texture2;


	bool hasSecond = false;
	char* filename;

	switch (t)
	{
		case ORBIT_TEXTURE: 
		{
			//hasSecond = true;
			//D3DX11CreateShaderResourceViewFromFile(m_device, L"../Engine/data/seafloor.dds", NULL, NULL, &texture, NULL); 
			result = CreateDDSTextureFromFile(m_device, L"../Engine/data/crystal.dds", nullptr, &texture, 0 , nullptr);
			if (FAILED(result))
				return false;

			/*result = CreateDDSTextureFromFile(m_device, L"../Engine/data/crystalNorm.dds", nullptr, &texture2, 0, nullptr);
			if (FAILED(result))
				return false;*/


			break;
		}
		case SKY_CUBE_TEXTURE:
		{
			bool res;
			/*D3DX11_IMAGE_LOAD_INFO skyMapInfo;
			skyMapInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;*/

			D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

			ID3D11Texture2D* texture_sky = 0;
			res = m_device->CreateTexture2D(&textureDesc, 0, &texture_sky);
			if (FAILED(res))
				return false;
			
			//res = D3DX11CreateTextureFromFile(m_device, L"../Engine/data/snowcube1024.dds", &skyMapInfo, 0, (ID3D11Resource**)&texture_sky, 0);
			res = CreateDDSTextureFromFile(m_device, L"../Engine/data/snowcube1024.dds", (ID3D11Resource**)&texture_sky, nullptr);
			if (FAILED(res))
			{
				return false;
			}

			D3D11_TEXTURE2D_DESC skyMapTextureDesc;
			texture_sky->GetDesc(&skyMapTextureDesc);

			D3D11_SHADER_RESOURCE_VIEW_DESC skyMapViewDesc;
			skyMapViewDesc.Format = skyMapTextureDesc.Format;
			skyMapViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			skyMapViewDesc.TextureCube.MipLevels = skyMapTextureDesc.MipLevels;
			skyMapViewDesc.TextureCube.MostDetailedMip = 0;

			res = m_device->CreateShaderResourceView(texture_sky, &skyMapViewDesc, &texture);
			if (FAILED(res))
			{
				return false;
			}
			break;
		}
		case TERRAIN_TEXTURE:
		{
			//D3DX11CreateShaderResourceViewFromFile(m_device, L"../Engine/data/seafloor.dds", NULL, NULL, &texture, NULL); 
			result = CreateDDSTextureFromFile(m_device, L"../Engine/data/forestfloor.dds", nullptr, &texture, 0, nullptr);
			if (FAILED(result))
				return false;
			break;
		}
		case TREE_TEXTURE:
		{	
			//hasSecond = true;
			//D3DX11CreateShaderResourceViewFromFile(m_device, L"../Engine/data/seafloor.dds", NULL, NULL, &texture, NULL); 
			result = CreateDDSTextureFromFile(m_device, L"../Engine/data/treebark.dds", nullptr, &texture, 0, nullptr);
			if (FAILED(result))
				return false;

			/*result = CreateDDSTextureFromFile(m_device, L"../Engine/data/treebark2Norm.dds", nullptr, &texture2, 0, nullptr);
			if (FAILED(result))
				return false;*/
			
			break;
		}
	}

	m_textures.push_back(new ID3D11ShaderResourceView*[2]);
	m_textures[m_textureCount][0] = texture;
	if (hasSecond)
	{
		m_textures[m_textureCount][1] = texture2;
	}
	++m_textureCount;
	return true;
}

bool ResourceManager::LoadModel(ModelResource m)
{
	std::ifstream fin;
	char input;
	int i;
	char* filename;
	Model* model = new Model();
	bool incInds = false;

	switch (m)
	{
		case REFLECTIVE_MODEL: filename = "../Engine/data/sphere.txt"; break;
		case ORBIT_MODEL: filename = "../Engine/data/sphere.txt"; break;
		case SKY_DOME_MODEL: filename = "../Engine/data/skydome.txt"; break;
		//case TREE_MODEL: filename = "../Engine/data/test.txt"; incInds = true; break;
	}

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	int vertexCount, indexCount;
	// Read in the vertex count.
	fin >> vertexCount;

	// Set the number of indices to be the same as the vertex count.
	indexCount = vertexCount;

	// Create the model using the vertex count that was read in.
	std::vector<Model::VertexType> data;
	data.resize(vertexCount);
	//Model::ModelData* data = new Model::ModelData[vertexCount];

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < vertexCount; i++)
	{
		fin >> data[i].position.x >> data[i].position.y >> data[i].position.z;
		fin >> data[i].texture.x >> data[i].texture.y;
		fin >> data[i].normal.x >> data[i].normal.y >> data[i].normal.z;
	}

	if (incInds)
	{
		std::vector<unsigned int> indices;
		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}

		fin >> indexCount;
		indices.resize(indexCount);

		fin.get(input);
		while (input != ':')
		{
			fin.get(input);
		}
		fin.get(input);
		fin.get(input);

		for (i = 0; i < indexCount; ++i)
		{
			fin >> indices[i];
		}
		
		model->SetModelData(data, indices);
	}
	else
	{
		model->SetModelData(data);
	}

	m_models.push_back(model);
	// Close the model file.
	fin.close();

	return true;
}



