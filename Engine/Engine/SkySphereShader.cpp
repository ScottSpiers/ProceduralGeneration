
#include "SkySphereShader.h"

SkySphereShader::SkySphereShader()
{
	m_srv = 0;
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleStateSky = 0;
	m_matrixBuffer = 0;
}

SkySphereShader::SkySphereShader(const SkySphereShader& other)
{
}

SkySphereShader::~SkySphereShader()
{
}

bool SkySphereShader::Initialise(ID3D11Device* device, HWND hwnd)
{
	bool res;

	res = InitialiseShader(device, hwnd, L"../Engine/skysphere.vs", L"../Engine/skysphere.ps");
	if (!res)
	{
		return false;
	}
	return true;
}

void SkySphereShader::Shutdown()
{
	ShutdownShader();
}

bool SkySphereShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool res;

	res = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!res)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);
	return true;
}

bool SkySphereShader::InitialiseShader(ID3D11Device* device, HWND hwnd, WCHAR* vs, WCHAR* ps)
{
	HRESULT res;
	ID3D10Blob* errorMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC gradientBufferDesc;
	D3D11_SAMPLER_DESC skySamplerDesc;

	errorMsg = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	res = D3DX11CompileFromFile(vs, NULL, NULL, "SkySphereVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMsg, NULL);
	if (FAILED(res))
	{
		if (errorMsg)
			OutputShaderErrorMessage(errorMsg, hwnd, vs);
		else
			MessageBox(hwnd, vs, L"Missing Shader File", MB_OK);
		return false;
	}

	res = D3DX11CompileFromFile(ps, NULL, NULL, "SkySpherePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMsg, NULL);
	if (FAILED(res))
	{
		if (errorMsg)
			OutputShaderErrorMessage(errorMsg, hwnd, ps);
		else
			MessageBox(hwnd, vs, L"Missing Shader File", MB_OK);
		return false;
	}

	res = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(res))
	{
		return false;
	}

	res = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(res))
	{
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	res = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(res))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//TALKING POINT
	D3DX11_IMAGE_LOAD_INFO skyMapInfo;
	skyMapInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* texture_sky = 0;
	res = D3DX11CreateTextureFromFile(device, L"../Engine/data/snowcube1024.dds", &skyMapInfo, 0, (ID3D11Resource**)&texture_sky, 0);
	if (FAILED(res))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC skyMapTextureDesc;
	texture_sky->GetDesc(&skyMapTextureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC skyMapViewDesc;
	skyMapViewDesc.Format = skyMapTextureDesc.Format;
	skyMapViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	skyMapViewDesc.TextureCube.MipLevels = skyMapTextureDesc.MipLevels;
	skyMapViewDesc.TextureCube.MostDetailedMip = 0;

	res = device->CreateShaderResourceView(texture_sky, &skyMapViewDesc, &m_srv);
	if (FAILED(res))
	{
		return false;
	}

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

	res = device->CreateSamplerState(&skySamplerDesc, &m_sampleStateSky);
	if (FAILED(res))
	{
		return false;
	}

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	res = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(res))
	{
		return false;
	}

	return true;
}

void SkySphereShader::ShutdownShader()
{

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_sampleStateSky)
	{
		m_sampleStateSky->Release();
		m_sampleStateSky = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_srv)
	{
		m_srv->Release();
		m_srv = 0;
	}
}

void SkySphereShader::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shader)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;

	compileErrors = (char*)(errorMsg->GetBufferPointer());

	bufferSize = errorMsg->GetBufferSize();

	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMsg->Release();
	errorMsg = 0;
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shader, MB_OK);

}

bool SkySphereShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	GradientBufferType* dataPtr2;
	unsigned int bufferNumber;

	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	res = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(res))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);


	//TALKING POINT
	deviceContext->PSSetShaderResources(0, 1, &m_srv);//this bit is rather important
	return true;
}

void SkySphereShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);


	//TALKING POINT
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateSky); //this is also quite important

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
