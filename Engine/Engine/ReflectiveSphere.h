#pragma once

#include "modelclass.h"

class ReflectiveSphere : public ModelClass
{

public:
	ReflectiveSphere();
	ReflectiveSphere(const ReflectiveSphere&);
	ReflectiveSphere(char*);
	~ReflectiveSphere();
	bool Initialize(ID3D11Device*, WCHAR*);
	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	TextureClass* m_Texture;
};