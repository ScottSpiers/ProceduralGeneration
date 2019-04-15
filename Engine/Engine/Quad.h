#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>

using namespace DirectX;

class Quad
{
private:
	struct Vertex
	{
		Vertex() : pos(), tex() {};
		Vertex(XMFLOAT3 p, XMFLOAT2 t) : pos(p), tex(t) {};

		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};

public:
	Quad(int, int);
	~Quad();

	bool Initialise(ID3D11Device*);
	void Render(ID3D11DeviceContext*);

	void SetTexture(ID3D11ShaderResourceView*);
	ID3D11ShaderResourceView* GetTexture();
	void SetWorldMatrix(XMMATRIX);
	XMMATRIX GetWorldMatrix();

	int GetWidth();
	int GetHeight();
	int GetIndexCount();

private:
	ID3D11Buffer* m_vBuffer;
	ID3D11Buffer* m_iBuffer;
	

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	int m_width;
	int m_height;

	XMMATRIX m_worldMatrix;
	ID3D11ShaderResourceView* m_texture;
};