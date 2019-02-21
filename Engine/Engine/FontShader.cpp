
#include "FontShader.h"

FontShader::FontShader()
{
	m_vertexShader = 0;
	m_pixelBuffer = 0;
	m_layout = 0;
	m_pixelBuffer = 0;
	m_constantBuffer = 0;
	m_sampleState = 0;
}

FontShader::FontShader(const FontShader&)
{
}

FontShader::~FontShader()
{
}

bool FontShader::Initialise(ID3D11Device* device, HWND hwnd)
{
	return InitialiseShader(device, hwnd, L"../Engine/Font.vs", L"../Engine/Font.ps");
}

void FontShader::Shutdown()
{
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = 0;
	}

	if (m_pixelBuffer)
	{
		m_pixelBuffer->Release();
		m_pixelBuffer = 0;
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
}

bool FontShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColour)
{
	if (!setShaderParams(deviceContext, world, view, projection, texture, pixelColour))
		return false;

	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->DrawIndexed(indexCount, 0, 0);
	return true;
}

bool FontShader::InitialiseShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
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

	res = D3DX11CompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &buf_vertexShader, &msg_error, NULL);
	if (FAILED(res))
	{
		if (msg_error)
		{
			OutputShaderErrorMessage(msg_error, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	res = D3DX11CompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &buf_pixelShader, &msg_error, NULL);
	if (FAILED(res))
	{
		if (msg_error)
		{
			OutputShaderErrorMessage(msg_error, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	res = device->CreateVertexShader(buf_vertexShader->GetBufferPointer(), buf_vertexShader->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(res))
		return false;

	res = device->CreatePixelShader(buf_pixelShader->GetBufferPointer(), buf_pixelShader->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(res))
		return false;

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

	res = device->CreateInputLayout(polygonLayout, numElems, buf_vertexShader->GetBufferPointer(), buf_vertexShader->GetBufferSize(), &m_layout);
	if (FAILED(res))
		return false;

	buf_vertexShader->Release();
	buf_vertexShader = 0;
	buf_pixelShader->Release();
	buf_pixelShader = 0;

	desc_constantBuffer.Usage = D3D11_USAGE_DYNAMIC;
	desc_constantBuffer.ByteWidth = sizeof(ConstantBufferType);
	desc_constantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc_constantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc_constantBuffer.MiscFlags = 0;
	desc_constantBuffer.StructureByteStride = 0;

	res = device->CreateBuffer(&desc_constantBuffer, NULL, &m_constantBuffer);
	if (FAILED(res))
		return false;

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

void FontShader::OutputShaderErrorMessage(ID3D10Blob* msg_error, HWND hwnd, WCHAR* shaderFile)
{
	char* compileErrors;
	unsigned long bufferSize;
	std::ofstream fout;

	compileErrors = (char*)(msg_error->GetBufferPointer());
	bufferSize = msg_error->GetBufferSize();
	fout.open("shader-error.txt");
	for (int i = 0; i < bufferSize; ++i)
	{
		fout << compileErrors[i];
	}
	fout.close();

	msg_error->Release();
	msg_error = 0;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message", shaderFile, MB_OK);
}

bool FontShader::setShaderParams(ID3D11DeviceContext* deviceContext, XMMATRIX world, XMMATRIX view, XMMATRIX projection, ID3D11ShaderResourceView* texture, XMFLOAT4 pixelColour)
{
	HRESULT res;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	PixelBufferType* dataPtr2;
	unsigned int bufferNumber;

	res = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(res))
		return false;

	dataPtr = (ConstantBufferType*)mappedResource.pData;
	
	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	projection = XMMatrixTranspose(projection);

	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = projection;

	deviceContext->Unmap(m_constantBuffer, 0);

	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	res = deviceContext->Map(m_pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(res))
		return false;

	dataPtr2 = (PixelBufferType*)mappedResource.pData;
	dataPtr2->pixelColour = pixelColour;

	deviceContext->Unmap(m_pixelBuffer, 0);
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_pixelBuffer);
	return true;
}