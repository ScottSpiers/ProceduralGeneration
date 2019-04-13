#pragma once

#include "Quad.h"
#include "Shader.h"

class TextureShader : public Shader
{

public:
	TextureShader(ID3D11Device*, ID3D11DeviceContext*);
	~TextureShader();

	bool Initialise();
	bool Render(Quad*, XMMATRIX, XMMATRIX);

private:
	ID3D11SamplerState* m_samplerState;

};