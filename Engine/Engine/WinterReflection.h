#pragma once

#include "Cubemap.h"
#include "Scene.h"

class WinterReflection final : public Scene
{
public:
	WinterReflection();
	~WinterReflection();

	virtual bool Initialise(ID3D11Device*, ID3D11DeviceContext*) final;
	virtual bool Render(D3D*) final;

private:
	bool RenderCubemap(D3D*);
	bool RenderScene(D3D*, Camera* cam, bool drawMirror);

private:
	Cubemap* m_cMap;
	ID3D11ShaderResourceView* m_cMapSRV;

	float m_passingTime;
};