#pragma once

#pragma once

#include "Light.h"
#include "Shader.h"
#include "LTree.h"

class LTreeShader : public Shader
{
private:

	struct LightBufferType
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT3 lightDirection;
		float pad;
	};

public:
	LTreeShader(ID3D11Device*, ID3D11DeviceContext*);
	~LTreeShader();

	bool Initialise();
	bool Render(LTree*, Camera*, Light*);
	bool Render(Model*, Camera*, Light*);

private:
	ID3D11Buffer* m_lightBuffer;
	ID3D11SamplerState* m_sampleState;
};