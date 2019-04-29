
#include "SkySphereShader.h"

SkySphereShader::SkySphereShader(ID3D11Device* device, ID3D11DeviceContext* context) : Shader(device, context)
{
	m_sampleStateSky = 0;
	
	
}

SkySphereShader::~SkySphereShader()
{
	if (m_sampleStateSky)
	{
		m_sampleStateSky->Release();
		m_sampleStateSky = 0;
	}
}

bool SkySphereShader::Initialise()
{
	HRESULT res;
	ID3D10Blob* errorMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElems;
	D3D11_BUFFER_DESC dnBufDesc;
	D3D11_SAMPLER_DESC skySamplerDesc;

	errorMsg = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	numElems = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	Shader::Initialise(L"skysphere.vs", "SkySphereVertexShader", L"skysphere.ps", "SkySpherePixelShader", polygonLayout, numElems);

	dnBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	dnBufDesc.ByteWidth = sizeof(DayNightBuffer);
	dnBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dnBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dnBufDesc.MiscFlags = 0;
	dnBufDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	res = m_device->CreateBuffer(&dnBufDesc, nullptr, &dnBuffer);
	if (FAILED(res))
	{
		return false;
	}

	////TALKING POINT
	//D3DX11_IMAGE_LOAD_INFO skyMapInfo;
	//skyMapInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	//ID3D11Texture2D* texture_sky = 0;
	//res = D3DX11CreateTextureFromFile(device, L"../Engine/data/snowcube1024.dds", &skyMapInfo, 0, (ID3D11Resource**)&texture_sky, 0);
	//if (FAILED(res))
	//{
	//	return false;
	//}

	//D3D11_TEXTURE2D_DESC skyMapTextureDesc;
	//texture_sky->GetDesc(&skyMapTextureDesc);

	//D3D11_SHADER_RESOURCE_VIEW_DESC skyMapViewDesc;
	//skyMapViewDesc.Format = skyMapTextureDesc.Format;
	//skyMapViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	//skyMapViewDesc.TextureCube.MipLevels = skyMapTextureDesc.MipLevels;
	//skyMapViewDesc.TextureCube.MostDetailedMip = 0;

	//res = device->CreateShaderResourceView(texture_sky, &skyMapViewDesc, &m_srv);
	//if (FAILED(res))
	//{
	//	return false;
	//}

	skySamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	skySamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	skySamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	skySamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	skySamplerDesc.MipLODBias = 0.0f;
	skySamplerDesc.MaxAnisotropy = 1;
	skySamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	skySamplerDesc.BorderColor[0] = 0;
	skySamplerDesc.BorderColor[1] = 0;
	skySamplerDesc.BorderColor[2] = 0;
	skySamplerDesc.BorderColor[3] = 0;
	skySamplerDesc.MinLOD = 0;
	skySamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	res = m_device->CreateSamplerState(&skySamplerDesc, &m_sampleStateSky);
	if (FAILED(res))
	{
		return false;
	}

	return true;
}

bool SkySphereShader::Render(Model* model, Camera* cam, float cWeight)
{
	HRESULT res;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DayNightBuffer* dataPtr;
	unsigned int bufferNumber;

	Shader::SetMatrixBuffer(model->GetWorldMatrix(), cam);

	res = m_context->Map(dnBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(res))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (DayNightBuffer*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.

	//XMFLOAT4 red(1.f, 0.f, 0.f, 1.f);
	XMFLOAT4 c = XMFLOAT4(cWeight, cWeight, cWeight, 1.0f);
	dataPtr->colourWeight = c;

	// Unlock the constant buffer.
	m_context->Unmap(dnBuffer, 0);
	
	m_context->PSSetConstantBuffers(0, 1, &dnBuffer);

	//TALKING POINT
	ID3D11ShaderResourceView** textures = model->GetTextures();
	int elems = sizeof(textures) / sizeof(textures[0]);
	m_context->PSSetShaderResources(0, elems, textures);//this bit is rather important


	m_context->IASetInputLayout(m_inputLayout);
	m_context->VSSetShader(m_vShader, NULL, 0);
	m_context->PSSetShader(m_pShader, NULL, 0);


	//TALKING POINT
	m_context->PSSetSamplers(0, 1, &m_sampleStateSky); //this is also quite important

	m_context->DrawIndexed(model->GetIndexCount(), 0, 0);
	return true;
}

