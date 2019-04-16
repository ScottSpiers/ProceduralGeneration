#pragma once

#include <directxmath.h>
#include <vector>

using namespace DirectX;

class Cylinder
{
private:
	struct Vertex
	{
		Vertex() {};
		Vertex(XMFLOAT3 position) : pos(position) {};
		Vertex(XMFLOAT3 position, XMFLOAT3 n, XMFLOAT2 texCoord) : pos(position), normal(n), tex(texCoord) {};

		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	Cylinder();
	Cylinder(int, int, int);
	~Cylinder();

	void GenCaps(float, float, int, bool);
	void GenCylinder(float, float, int);
	void Rotate(XMMATRIX);
	void Translate(XMVECTOR);

	//This Will change when VertexType is out into a header of its own
	XMFLOAT3 GetPosition(int);
	XMFLOAT2 GetTexCoord(int);
	XMFLOAT3 GetNormal(int);

	int GetNumVertices();
	std::vector<unsigned int> GetIndices();

private:
	inline XMVECTOR GetCircleVector(int, int);

private:
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
};