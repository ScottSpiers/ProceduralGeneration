
#include "TerrainShader.h"

TerrainShader::TerrainShader(ID3D11Device* device, ID3D11DeviceContext* context) : Shader(device, context)
{

	m_lightBuffer = 0;
	m_sampleState = 0;
}

TerrainShader::~TerrainShader()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
}

bool TerrainShader::Initialise()
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElems;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC camBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElems = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = Shader::Initialise(L"Terrain.vs", "TerrainVertexShader", L"Terrain.ps", "TerrainPixelShader", polygonLayout, numElems);
	//result = Shader::Initialise(L"Light.vs", "LightVertexShader", L"Light.ps", "LightPixelShader", polygonLayout, numElems);
	if (FAILED(result))
		return false;

	// Create a texture sampler state description.
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
	result = m_device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = m_device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool TerrainShader::Render(Terrain* t, Camera* cam, Light* light)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	LightBufferType* dataPtr;

	Shader::SetMatrixBuffer(t->GetWorldMatrix(), cam);

	// Lock the light constant buffer so it can be written to.
	result = m_context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.

	//XMFLOAT4 red(1.f, 0.f, 0.f, 1.f);
	dataPtr->ambientColour = light->GetAmbientColour();
	dataPtr->diffuseColour = light->GetDiffuseColor();
	dataPtr->lightDirection = light->GetDirection();
	dataPtr->pad = 0.0f;

	// Unlock the constant buffer.
	m_context->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	m_context->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	ID3D11ShaderResourceView** textures = t->GetTextures();
	int elems = sizeof(textures) / sizeof(textures[0]);
	m_context->PSSetShaderResources(0, elems, textures);

	// Set the vertex input layout.
	m_context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_context->VSSetShader(m_vShader, NULL, 0);
	m_context->PSSetShader(m_pShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	m_context->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	m_context->DrawIndexed(t->GetIndexCount(), 0, 0);

	return true;
}