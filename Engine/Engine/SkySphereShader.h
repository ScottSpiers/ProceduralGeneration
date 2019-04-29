#pragma once


#include <D3D11.h>
#include <DirectXMath.h>
#include <fstream>

#include "Shader.h"
#include "Model.h"

using namespace DirectX;

class SkySphereShader : public Shader
{
	
private:
	struct DayNightBuffer
	{
		XMFLOAT4 colourWeight;
	};

public:
	SkySphereShader(ID3D11Device*, ID3D11DeviceContext*);
	~SkySphereShader();

	bool Initialise();
	bool Render(Model*, Camera*, float);


private:
	ID3D11Buffer* dnBuffer;
	ID3D11SamplerState* m_sampleStateSky;
};