#pragma once


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: LightShader
////////////////////////////////////////////////////////////////////////////////
class ReflectShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPos;
		float pad;
	};

	struct VariableBufferType
	{
		float delta;
		D3DXVECTOR3 padding;
	};


	struct LightBufferType
	{
		D3DXVECTOR4 ambientColour;
		D3DXVECTOR4 diffuseColour;
		D3DXVECTOR3 lightDirection;
		float specularIntensity;
		D3DXVECTOR4 specColour;
	};

public:
	ReflectShader();
	ReflectShader(const ReflectShader&);
	~ReflectShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, ID3D11ShaderResourceView*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX , D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float, ID3D11ShaderResourceView*);
	void ReleaseShaderParams(ID3D11DeviceContext*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, ID3D11ShaderResourceView*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_variableBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;
};