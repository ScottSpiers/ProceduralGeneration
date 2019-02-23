#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

#include "Light.h"
#include "Model.h"
#include "Shader.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightShader
////////////////////////////////////////////////////////////////////////////////
class LightShader : public Shader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float pad;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT3 lightDirection;
		float specularIntensity;
		XMFLOAT4 specColour;
	};

public:
	LightShader(ID3D11Device*, ID3D11DeviceContext*);
	~LightShader();

	bool Initialise();
	
	bool Render(Model* model, Camera* cam, Light* light);

private:
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};