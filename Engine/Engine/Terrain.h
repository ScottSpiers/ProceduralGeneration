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
		XMVECTOR pos;
		XMVECTOR normal;
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

private:
	void CalcNormals();


private:
	ID3D11Buffer *m_vBuffer, *m_iBuffer;

	int m_width, m_height, m_vCount, m_iCount;
	bool m_toggleGenerated;

	std::vector<VertexType> m_vertices;
	std::vector<int> m_indices;

	XMMATRIX m_worldMatrix;

};