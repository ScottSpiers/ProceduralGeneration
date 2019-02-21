#pragma once


#include <D3D11.h>
#include <DirectXMath.h>
#include <D3DX11async.h>
#include <fstream>

#include "Shader.h"
#include "Model.h"

using namespace DirectX;

class SkySphereShader : public Shader
{
	
public:
	SkySphereShader(ID3D11Device*, ID3D11DeviceContext*);
	~SkySphereShader();

	bool Initialise();
	bool Render(Model*, Camera*);


private:
	ID3D11SamplerState* m_sampleStateSky;
};