#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <time.h>

using namespace DirectX;

class Terrain
{
private:
	struct VertexType
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	Terrain(int, int);
	~Terrain();

	bool Initialise(ID3D11Device*);
	bool Render(ID3D11DeviceContext*);

	void GenRandom();
	void GenSinWave();

	XMMATRIX GetWorldMatrix();
	int GetIndexCount();

	void SetTexture(ID3D11ShaderResourceView*);
	ID3D11ShaderResourceView* GetTexture();
	float GetTerrainHeight(float, float);

private:
	void CalcNormals();
	void CalcTexCoords();


private:
	ID3D11Buffer *m_vBuffer, *m_iBuffer;
	ID3D11ShaderResourceView* m_texture;

	int m_width, m_height, m_vCount, m_iCount;
	bool m_toggleGenerated;

	std::vector<VertexType> m_vertices;
	std::vector<int> m_indices;

	XMMATRIX m_worldMatrix;

};