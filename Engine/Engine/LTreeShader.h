#pragma once

#pragma once

#include "Light.h"
#include "Shader.h"
#include "LTree.h"

class LTreeShader : public Shader
{
private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float pad; //AAAAAAAAAAAAAGH!
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT3 lightDirection;
		float specIntensity;
		XMFLOAT4 specColour;
	};

public:
	LTreeShader(ID3D11Device*, ID3D11DeviceContext*);
	~LTreeShader();

	bool Initialise();
	bool Render(LTree*, Camera*, Light*);

private:
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_camBuffer;
};