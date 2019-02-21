
#include "Cubemap.h"

Cubemap::Cubemap()
{
	for (int i = 0; i < 6; ++i)
	{
		m_dynamicCubeRTV[i] = 0;
	}

	m_depthStencilView = 0;
	m_dynamicCubeSRV = 0;
	m_texture = 0;
	m_depthTexture = 0;
}

void Cubemap::Initialise(ID3D11Device* device)
{
	BuildViews(device);
	InitCameras(0,0,0);
}

void Cubemap::Shutdown()
{
	if (m_depthTexture)
	{
		m_depthTexture->Release();
		m_depthTexture = 0;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (m_dynamicCubeSRV)
	{
		m_dynamicCubeSRV->Release();
		m_dynamicCubeSRV = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	for (int i = 0; i < 6; ++i)
	{
		if (m_dynamicCubeRTV[i])
		{
			m_dynamicCubeRTV[i]->Release();
			m_dynamicCubeRTV[i] = 0;
		}
	}
}

void Cubemap::BuildViews(ID3D11Device* device)
{
	HRESULT result;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = CUBEMAP_SIZE;
	texDesc.Height = CUBEMAP_SIZE;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc = { 1, 0 };
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	result = device->CreateTexture2D(&texDesc, 0, &m_texture);
	if (FAILED(result))
	{
		return;
	}

	ID3D11Texture2D* cubeTex = 0;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		result = device->CreateRenderTargetView(m_texture, &rtvDesc, &(m_dynamicCubeRTV[i]));
		if (FAILED(result))
		{
			return;
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = texDesc.MipLevels;


	result = device->CreateShaderResourceView(m_texture, &srvDesc, &m_dynamicCubeSRV);
	if (FAILED(result))
	{
		return;
	}

	D3D11_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
	depthTexDesc.Width = CUBEMAP_SIZE;
	depthTexDesc.Height = CUBEMAP_SIZE;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc = { 1,0 };
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	result = device->CreateTexture2D(&depthTexDesc, 0, &m_depthTexture);
	if (FAILED(result))
	{
		return;
	}

	ID3D11Texture2D* depthTex = 0;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(m_depthTexture, &dsvDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return;
	}
	
	m_viewport.Height = (float)CUBEMAP_SIZE;
	m_viewport.Width = (float)CUBEMAP_SIZE;
	m_viewport.MinDepth = 0.f;
	m_viewport.MaxDepth = 1.f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
}

void Cubemap::InitCameras(float x, float y, float z)
{
	XMVECTOR centre{ x,y,z };
	XMVECTOR targets[6]
	{
		XMVECTOR{ x + 1, y, z },
		XMVECTOR{ x - 1, y, z },
		XMVECTOR{ x, y + 1, z },
		XMVECTOR{ x, y - 1, z },
		XMVECTOR{ x, y, z + 1 },
		XMVECTOR{ x, y, z - 1 }
	};

	XMVECTOR ups[6]
	{
		XMVECTOR{0,1,0},
		XMVECTOR{0,1,0},
		XMVECTOR{0,0,-1},
		XMVECTOR{0,0,1},
		XMVECTOR{0,1,0},
		XMVECTOR{0,1,0}
	};

	XMMATRIX view;
	m_proj = XMMatrixPerspectiveFovLH(XM_PI / 2, 1.0f, 0.1f, 200.f);

	for (int i = 0; i < 6; ++i)
	{
		view = XMMatrixLookAtLH(centre, targets[i], ups[i]);
		m_views[i].setViewMatrix(view);
		m_views[i].setProjMatrix(m_proj);
	}
}

Camera* Cubemap::getView(int i)
{
	return &m_views[i];
}

XMMATRIX Cubemap::getProjection() const
{
	return m_proj;
}

ID3D11ShaderResourceView* Cubemap::getSRV() const
{
	return m_dynamicCubeSRV;
}

ID3D11RenderTargetView* Cubemap::getRTV(int i) const
{
	return m_dynamicCubeRTV[i];
}

ID3D11DepthStencilView* Cubemap::getDSV() const
{
	return m_depthStencilView;
}

D3D11_VIEWPORT Cubemap::getViewport() const
{
	return m_viewport;
}