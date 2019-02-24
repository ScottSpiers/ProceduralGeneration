#pragma once


#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "Camera.h"

using namespace DirectX;

class Shader
{
public:
	struct MatrixBuffer
	{
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};

public:
	Shader(ID3D11Device*, ID3D11DeviceContext*);
	~Shader();

	bool Initialise(WCHAR*, LPCSTR, WCHAR*, LPCSTR, D3D11_INPUT_ELEMENT_DESC[], UINT);

protected:
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool CompileShaderFromFile(WCHAR*, LPCSTR, LPCSTR, ID3DBlob**);

	bool SetMatrixBuffer(XMMATRIX, Camera*);
	bool SetMatrixBuffer(XMMATRIX, XMMATRIX, XMMATRIX);

protected:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	ID3D11VertexShader* m_vShader;
	ID3D11PixelShader* m_pShader;
	ID3D11InputLayout* m_inputLayout;
	ID3D11Buffer* m_matrixBuffer;

};