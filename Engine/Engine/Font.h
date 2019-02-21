#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;

///Heavily influenced by rastertek tutorial on Font
class Font
{
private:
	struct FontType
	{
		float left;
		float right;
		int size;
	};

	struct VertexType
	{
		XMFLOAT3 pos;
		XMFLOAT2 texture;
	};

public:
	Font();
	Font(const Font&);
	~Font();

	bool Initialise(ID3D11Device*, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* getTexture();
	void buildVertexArray(void*, const char*, float, float);

private:
	bool loadFont(char*);
	bool loadTexture(ID3D11Device*, WCHAR*);

private:
	FontType* m_font;
	ID3D11ShaderResourceView* m_texture;
};