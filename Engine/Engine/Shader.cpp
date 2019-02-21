
#include <fstream>

#include "Shader.h"

Shader::Shader(ID3D11Device* device, ID3D11DeviceContext* context) : m_device(device), m_context(context)
{
	m_vShader = 0;
	m_pShader = 0;
	m_inputLayout = 0;
	m_matrixBuffer = 0;
}

Shader::~Shader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_inputLayout)
	{
		m_inputLayout->Release();
		m_inputLayout = 0;
	}

	if (m_pShader)
	{
		m_pShader->Release();
		m_pShader = 0;
	}

	if (m_vShader)
	{
		m_vShader->Release();
		m_vShader = 0;
	}

}

bool Shader::Initialise(WCHAR* vsFilename, LPCSTR vsEntry, WCHAR* psFilename, LPCSTR psEntry, D3D11_INPUT_ELEMENT_DESC polygonLayout[], UINT numElems)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = CompileShaderFromFile(vsFilename, vsEntry, "vs_5_0", &vertexShaderBuffer);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, 0, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(0, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = CompileShaderFromFile(psFilename, psEntry, "ps_5_0", &pixelShaderBuffer);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, 0, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(0, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout.
	result = m_device->CreateInputLayout(polygonLayout, numElems, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_inputLayout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

HRESULT Shader::CompileShaderFromFile(WCHAR* filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob** shader)
{
	HRESULT result;
	ID3DBlob* errorMsg;

	result = D3DCompileFromFile(filename, nullptr, nullptr, entryPoint, target, D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, shader, &errorMsg);
	
	
	return S_OK;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shader)
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

bool Shader::SetMatrixBuffer(XMMATRIX worldMatrix, Camera* cam)
{

	MatrixBuffer* matBufData;
	bool result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	result = m_context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (!result)
	{
		return false;
	}

	matBufData = (MatrixBuffer*)mappedResource.pData;

	matBufData->worldMatrix = XMMatrixTranspose(worldMatrix);
	matBufData->viewMatrix = XMMatrixTranspose(cam->GetViewMatrix());
	matBufData->projectionMatrix = XMMatrixTranspose(cam->GetProjMatrix());

	m_context->Unmap(m_matrixBuffer, 0);
	m_context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
}