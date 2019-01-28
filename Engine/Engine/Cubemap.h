#pragma once

#include "d3dclass.h"
#include "cameraclass.h"
#include <DirectXMath.h>
#include <D3D11.h>


class Cubemap
{

public:
	Cubemap();
	void Initialise(ID3D11Device*);
	void Shutdown();
	D3D11_VIEWPORT getViewport() const;
	CameraClass* getView(int i);
	D3DXMATRIX getProjection() const;
	ID3D11RenderTargetView* getRTV(int i) const;
	ID3D11DepthStencilView* getDSV() const;
	ID3D11ShaderResourceView* getSRV() const;

	void InitCameras(float, float, float);

private:
	void BuildViews(ID3D11Device*);


private:
	const int CUBEMAP_SIZE = 256;

	CameraClass m_views[6];
	D3DXMATRIX m_proj;
	D3D11_VIEWPORT m_viewport;
	ID3D11Texture2D* m_texture;
	ID3D11Texture2D* m_depthTexture;
	ID3D11RenderTargetView* m_dynamicCubeRTV[6];
	ID3D11ShaderResourceView* m_dynamicCubeSRV;
	ID3D11DepthStencilView* m_depthStencilView;

};