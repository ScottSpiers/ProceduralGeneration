#pragma once


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>


#include "Model.h"
#include "Shader.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightShader
////////////////////////////////////////////////////////////////////////////////
class ReflectShader : public Shader
{
private:
	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float pad;
	};

public:
	ReflectShader(ID3D11Device*, ID3D11DeviceContext*);
	~ReflectShader();

	bool Initialise();
	bool Render(Model*, Camera*);


private:
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};