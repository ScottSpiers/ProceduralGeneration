
#include "FontShader.h"

FontShader::FontShader(ID3D11Device* device, ID3D11DeviceContext* context) : Shader(device, context)
{
	m_pixelBuffer = 0;
	m_sampleState = 0;
}


FontShader::~FontShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
	}
}

bool FontShader::Initialise(ID3D11Device* device, HWND hwnd)
{
	HRESULT res;
	ID3D10Blob* msg_error;
	ID3D10Blob* buf_vertexShader;
	ID3D10Blob* buf_pixelShader;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElems;
	D3D11_BUFFER_DESC desc_constantBuffer;
	D3D11_BUFFER_DESC desc_pixelBuffer;
	D3D11_SAMPLER_DESC desc_sampler;

	msg_error = 0;
	buf_vertexShader = 0;
	buf_pixelShader = 0;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElems = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	Shader::Initialise(L"Font.vs", "FontVertexShader", L"Font.ps", "FontPixelShader", polygonLayout, numElems);

	desc_sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc_sampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc_sampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc_sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc_sampler.MipLODBias = 0.0f;
	desc_sampler.MaxAnisotropy = 1;
	desc_sampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc_sampler.BorderColor[0] = 0;
	desc_sampler.BorderColor[1] = 0;
	desc_sampler.BorderColor[2] = 0;
	desc_sampler.BorderColor[3] = 0;
	desc_sampler.MinLOD = 0;
	desc_sampler.MaxLOD = D3D11_FLOAT32_MAX;

	res = device->CreateSamplerState(&desc_sampler, &m_sampleState);
	if (FAILED(res))
		return false;

	desc_pixelBuffer.Usage = D3D11_USAGE_DYNAMIC;
	desc_pixelBuffer.ByteWidth = sizeof(PixelBufferType);
	desc_pixelBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc_pixelBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc_pixelBuffer.MiscFlags = 0;
	desc_pixelBuffer.StructureByteStride = 0;

	res = device->CreateBuffer(&desc_pixelBuffer, NULL, &m_pixelBuffer);
	if (FAILED(res))
		return false;

	return true;
}


bool FontShader::Render(int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColour)
{
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	PixelBufferType* dataPtr;
	unsigned int bufferNumber;

	Shader::SetMatrixBuffer(world, view, projection);

	res = m_context->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(res))
		return false;

	dataPtr = (PixelBufferType*)mappedResource.pData;
	dataPtr->pixelColour = pixelColour;

	m_context->Unmap(m_pixelBuffer, 0);
	bufferNumber = 0;

	m_context->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);
	//m_context->PSSetShaderResources(0, 1, &texture);

	m_context->IASetInputLayout(m_inputLayout);
	m_context->VSSetShader(m_vShader, NULL, 0);
	m_context->PSSetShader(m_pShader, NULL, 0);
	m_context->PSSetSamplers(0, 1, &m_sampleState);
	m_context->DrawIndexed(indexCount, 0, 0);
	return true;
}

