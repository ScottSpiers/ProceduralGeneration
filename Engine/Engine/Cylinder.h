#pragma once

#include <directxmath.h>
#include <vector>

using namespace DirectX;

class Cylinder
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	Cylinder();
	Cylinder(int, int, int);
	~Cylinder();

	void GenCaps(int, int, int, bool);
	void GenCylinder(int, int, int);
	void Rotate(XMMATRIX);
	void Translate(XMVECTOR);

	//This Will change when VertexType is out into a header of its own
	XMFLOAT3 GetPosition(int);
	XMFLOAT2 GetTexCoord(int);
	XMFLOAT3 GetNormal(int);

	int GetNumVertices();
	std::vector<int> GetIndices();

private:
	inline XMVECTOR GetCircleVector(int, int);

private:
	std::vector<Vertex> m_vertices;
	std::vector<int> m_indices;
};