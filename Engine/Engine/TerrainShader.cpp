
#include "TerrainShader.h"

TerrainShader::TerrainShader(ID3D11Device* device, ID3D11DeviceContext* context) : Shader(device, context)
{
	m_lightBuffer = 0;
	m_camBuffer = 0;
}

TerrainShader::~TerrainShader()
{
	if (m_camBuffer)
	{
		m_camBuffer->Release();
		m_camBuffer = 0;
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElems;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC camBufferDesc;

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

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElems = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = Shader::Initialise(L"Terrain.vs", "TerrainVertexShader", L"Terrain.ps", "TerrainPixelShader", polygonLayout, numElems);
	if (!result)
		return false;

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

	camBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	camBufferDesc.ByteWidth = sizeof(CameraBufferType);
	camBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	camBufferDesc.MiscFlags = 0;
	camBufferDesc.StructureByteStride = 0;

	result = m_device->CreateBuffer(&camBufferDesc, nullptr, &m_camBuffer);
	if (FAILED(result))
		return false;

	return true;
}

bool TerrainShader::Render(Terrain* t, Camera* cam, Light* light)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	LightBufferType* dataPtr;
	CameraBufferType* dataPtr2;

	Shader::SetMatrixBuffer(t->GetWorldMatrix(), cam);

	//Lock the camera constant buffer so it can be written to

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
	dataPtr->specColour = light->GetSpecColour();
	dataPtr->lightDirection = light->GetDirection();
	dataPtr->specIntensity = light->GetSpecIntensity(); // AAAAAAAAAAAAAGHHHHHH!

	// Unlock the constant buffer.
	m_context->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	m_context->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	result = m_context->Map(m_camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
		return false;

	dataPtr2 = (CameraBufferType*)mappedResource.pData;

	dataPtr2->cameraPos = cam->GetPosition();
	dataPtr2->pad = .0f; //AAAAAAAAAAAAAAAAAAAAGH
	m_context->Unmap(m_camBuffer, 0);
	//Don't forget the matrix buffer
	bufferNumber = 1;

	m_context->VSSetConstantBuffers(bufferNumber, 1, &m_camBuffer);
	/*ID3D11ShaderResourceView* texture = t->GetTexture();
	m_context->PSSetShaderResources(0, 1, &texture);*/

	// Set the vertex input layout.
	m_context->IASetInputLayout(m_inputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	m_context->VSSetShader(m_vShader, NULL, 0);
	m_context->PSSetShader(m_pShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	//m_context->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	m_context->DrawIndexed(t->GetIndexCount(), 0, 0);

	return true;
}