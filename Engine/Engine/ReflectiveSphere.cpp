
#include "ReflectiveSphere.h"


ReflectiveSphere::ReflectiveSphere() : ModelClass()
{
	m_Texture = 0;
}

ReflectiveSphere::ReflectiveSphere(const ReflectiveSphere& other)
{
}

ReflectiveSphere::ReflectiveSphere(char* filename) : ModelClass(filename)
{
	m_Texture = 0;
}

ReflectiveSphere::~ReflectiveSphere()
{
}

bool ReflectiveSphere::Initialize(ID3D11Device* device, WCHAR* textureFilename)
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

bool ReflectiveSphere::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
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

void ReflectiveSphere::Shutdown()
{

	// Shutdown the vertex and index buffers.
	ReleaseBuffers();

	// Release the model data.
	ReleaseModel();

	ReleaseTexture();
	return;
}

ID3D11ShaderResourceView* ReflectiveSphere::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ReflectiveSphere::LoadTexture(ID3D11Device* device, WCHAR* filename)
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

void ReflectiveSphere::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}