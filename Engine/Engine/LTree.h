#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <stack>
#include <vector>

using namespace DirectX;

class LTree
{
private:
	struct TurtleState
	{
		XMVECTOR pos;
		XMVECTOR dir = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
		XMMATRIX rotation;
	};

	//put this in a header! I'm using it everywhere
	struct VertexType
	{
		VertexType() {};
		VertexType(XMFLOAT3 position) : pos(position) {};

		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	LTree();
	~LTree();

	bool Initialise(ID3D11Device*);
	bool Render(ID3D11DeviceContext*);
	void InterpretSystem(std::string, int, float);

	XMMATRIX GetWorldMatrix();
	int GetIndexCount();

private:
	ID3D11Buffer* m_vBuffer, *m_iBuffer;
	std::vector<VertexType> m_vertices;
	std::vector<int> m_indices;

	XMMATRIX m_worldMatrix;

};