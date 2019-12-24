#pragma once

#include "Quad.h"
#include "Shader.h"

class MapShader : public Shader
{

private:

public:
	MapShader(ID3D11Device*, ID3D11DeviceContext*);
	~MapShader();

	bool Initialise();
	bool Render(Quad*, XMMATRIX, XMMATRIX);

private:
	ID3D11SamplerState* m_sampleState;
};