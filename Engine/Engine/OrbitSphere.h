//#pragma once
//
//#include "Model.h"
//#include "Texture.h"
//
//class OrbitSphere
//{
//public:
//	OrbitSphere();
//	OrbitSphere(const OrbitSphere&);
//	~OrbitSphere();
//	bool Initialize(ID3D11Device*, WCHAR*);
//	void Shutdown();
//
//	ID3D11ShaderResourceView* GetTexture();
//
//private:
//	bool LoadTexture(ID3D11Device*, WCHAR*);
//	void ReleaseTexture();
//
//private:
//	Model* m_model;
//	Texture* m_Texture;
//};