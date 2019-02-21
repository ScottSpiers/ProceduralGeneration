//
//#include "OrbitSphere.h"
//
//OrbitSphere::OrbitSphere()
//{
//	m_Texture = 0;
//}
//
//OrbitSphere::OrbitSphere(const OrbitSphere& other)
//{
//}
//
//OrbitSphere::~OrbitSphere()
//{
//}
//
//bool OrbitSphere::Initialize(ID3D11Device* device, WCHAR* textureFilename)
//{
//	bool result;
//
//	m_Texture = NULL;
//
//	result = LoadTexture(device, textureFilename);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//
//void OrbitSphere::Shutdown()
//{
//
//	// Shutdown the vertex and index buffers.
//	ReleaseBuffers();
//
//	// Release the model data.
//	ReleaseModel();
//
//	ReleaseTexture();
//	return;
//}
//
//ID3D11ShaderResourceView* OrbitSphere::GetTexture()
//{
//	return m_Texture->GetTexture();
//}
//
//bool OrbitSphere::LoadTexture(ID3D11Device* device, WCHAR* filename)
//{
//	bool result;
//
//	m_Texture = new Texture;
//	if (!m_Texture)
//	{
//		return false;
//	}
//
//	result = m_Texture->Initialize(device, filename);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//void OrbitSphere::ReleaseTexture()
//{
//	if (m_Texture)
//	{
//		m_Texture->Shutdown();
//		delete m_Texture;
//		m_Texture = 0;
//	}
//
//	return;
//}