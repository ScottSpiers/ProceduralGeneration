#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

#include "Shader.h"

using namespace DirectX;

class FontShader : Shader
{
private:
	struct PixelBufferType
	{
		XMFLOAT4 pixelColour;
	};

public:
	FontShader(ID3D11Device*, ID3D11DeviceContext*);
	~FontShader();

	bool Initialise(ID3D11Device*, HWND);
	bool Render(int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT4);

private:
	ID3D11Buffer* m_pixelBuffer;
	ID3D11SamplerState* m_sampleState;
};