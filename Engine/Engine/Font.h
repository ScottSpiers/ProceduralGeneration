#pragma once

#include "textureclass.h"

#include <D3D11.h>
#include <D3DX10math.h>
#include <fstream>

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
		D3DXVECTOR3 pos;
		D3DXVECTOR2 texture;
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
	TextureClass* m_texture;
};