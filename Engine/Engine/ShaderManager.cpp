
#include "ShaderManager.h"

ShaderManager::ShaderManager(ID3D11Device* device, ID3D11DeviceContext* context)
{
	m_lightShader = new LightShader(device, context);
	m_reflectShader = new ReflectShader(device, context);
	m_skyShader = new SkySphereShader(device, context);
	m_terrainShader = new TerrainShader(device, context);
	m_lTreeShader = new LTreeShader(device, context);
}

ShaderManager::~ShaderManager()
{
	if (m_lTreeShader)
	{
		delete m_lTreeShader;
		m_lTreeShader = 0;
	}

	if (m_terrainShader)
	{
		delete m_terrainShader;
		m_terrainShader = 0;
	}

	if (m_skyShader)
	{
		delete m_skyShader;
		m_skyShader = 0;
	}

	if (m_reflectShader)
	{
		delete m_reflectShader;
		m_reflectShader = 0;
	}
	
	if (m_lightShader)
	{
		delete m_lightShader;
		m_lightShader = 0;
	}
}

bool ShaderManager::InitialiseShaders()
{
	bool result;

	result = m_lightShader->Initialise();
	if (!result)
		return false;

	result = m_reflectShader->Initialise();
	if (!result)
		return false;

	result = m_skyShader->Initialise();
	if (!result)
		return false;

	result = m_terrainShader->Initialise();
	if (!result)
		return false;

	result = m_lTreeShader->Initialise();
	if (!result)
		return false;

	return true;
}

bool ShaderManager::RenderLight(Model* model, Camera* cam, Light* light)
{
	return m_lightShader->Render(model, cam, light);
}

bool ShaderManager::RenderReflection(Model* model, Camera* cam)
{
	return m_reflectShader->Render(model, cam);
}

bool ShaderManager::RenderSkySphere(Model* model, Camera* cam)
{
	return m_skyShader->Render(model, cam);
}

bool ShaderManager::RenderTerrain(Terrain* t, Camera* cam, Light* light)
{
	return m_terrainShader->Render(t, cam, light);
}

bool ShaderManager::RenderLTree(LTree* t, Camera* cam, Light* light)
{
	return m_lTreeShader->Render(t, cam, light);
}