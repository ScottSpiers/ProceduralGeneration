
#include "TextureShader.h"

TextureShader::TextureShader(ID3D11Device* device, ID3D11DeviceContext* context) : Shader(device, context)
{
	m_sampleState = 0;
}
TextureShader::~TextureShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}
}

bool TextureShader::Initialise()
{
	HRESULT res;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElems;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC screenBufDesc;

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

	res = Shader::Initialise(L"Texture.vs", "TextureVertexShader", L"Texture.ps", "TexturePixelShader", polygonLayout, numElems);
	if (FAILED(res))
		return false;

	screenBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenBufDesc.ByteWidth = sizeof(ScreenBuffer);
	screenBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenBufDesc.MiscFlags = 0;
	screenBufDesc.StructureByteStride = 0;

	res = m_device->CreateBuffer(&screenBufDesc, nullptr, &m_screenBuffer);


	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	res = m_device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(res))
		return false;

	return true;
}

bool TextureShader::Render(Quad* q, XMMATRIX view, XMMATRIX ortho)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	ScreenBuffer* dataPtr;

	Shader::SetMatrixBuffer(q->GetWorldMatrix(), view, ortho);

	result = m_context->Map(m_screenBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	dataPtr = (ScreenBuffer*)mappedResource.pData;

	dataPtr->screenDims = XMFLOAT2A(q->GetWidth(), q->GetHeight());
	m_context->Unmap(m_screenBuffer, 0);
	//Don't forget the matrix buffer
	bufferNumber = 1;

	m_context->VSSetConstantBuffers(bufferNumber, 1, &m_screenBuffer);

	ID3D11ShaderResourceView* texture = q->GetTexture();
	m_context->PSSetShaderResources(0, 1, &texture);

	// Set the vertex input layout.
	m_context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_context->VSSetShader(m_vShader, NULL, 0);
	m_context->PSSetShader(m_pShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	m_context->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	m_context->DrawIndexed(q->GetIndexCount(), 0, 0);

	return true;
}