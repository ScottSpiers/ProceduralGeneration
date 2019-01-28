
#include "OrbitSphere.h"

OrbitSphere::OrbitSphere() : ModelClass()
{
	m_Texture = 0;
}

OrbitSphere::OrbitSphere(const OrbitSphere& other)
{
}

OrbitSphere::OrbitSphere(char* filename) : ModelClass(filename)
{
	m_Texture = 0;
}

OrbitSphere::~OrbitSphere()
{
}

bool OrbitSphere::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	if (!m_filename)
		return false;

	// Load in the model data,
	result = LoadModel(m_filename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	m_Texture = NULL;

	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool OrbitSphere::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;


	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	m_Texture = NULL;

	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void OrbitSphere::Shutdown()
{

	// Shutdown the vertex and index buffers.
	ReleaseBuffers();

	// Release the model data.
	ReleaseModel();

	ReleaseTexture();
	return;
}

ID3D11ShaderResourceView* OrbitSphere::GetTexture()
{
	return m_Texture->GetTexture();
}

bool OrbitSphere::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void OrbitSphere::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}