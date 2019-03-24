
#include "Cylinder.h"

Cylinder::Cylinder(int radius, int height, int tesselation)
{
	GenCylinder(radius, height, tesselation);
}

Cylinder::~Cylinder()
{

}

//Implemented with help from https://github.com/Microsoft/DirectXTK/blob/master/Src/Geometry.cpp
inline XMVECTOR Cylinder::GetCircleVector(int i, int t)
{
	float angle = i * XM_2PI / t;
	float deltaX;
	float deltaZ;

	XMScalarSinCos(&deltaX, &deltaZ, angle);

	return XMVectorSet(deltaX, 0, deltaZ, 0);
}

void Cylinder::GenCylinder(int r, int h, int t)
{
	m_vertices.clear();
	m_indices.clear();

	int height = h / 2;

	XMVECTOR offsetTop = XMVectorScale(g_XMIdentityR1, height);

	int stride = t + 1;

	for (int i = 0; i <= t; ++i)
	{
		XMVECTOR normal = GetCircleVector(i, t);
		XMVECTOR offsetSide = XMVectorScale(normal, r);

		float u = float(i) / t;

		XMVECTOR texCoord = XMLoadFloat(&u);
		Vertex v;
		XMStoreFloat3(&v.pos, XMVectorAdd(offsetSide, offsetTop));
		XMStoreFloat3(&v.normal, normal);
		XMStoreFloat2(&v.tex, texCoord);
		m_vertices.push_back(v);

		XMStoreFloat3(&v.pos, XMVectorSubtract(offsetSide, offsetTop));
		XMStoreFloat3(&v.normal, normal);
		XMStoreFloat2(&v.tex, XMVectorAdd(texCoord, g_XMIdentityR1));
		
		
		//fully stolen from the link above
		m_indices.push_back(i * 2);
		m_indices.push_back((i * 2 + 2) % (stride * 2));
		m_indices.push_back(i * 2 + 1);

		m_indices.push_back(i * 2 + 1);
		m_indices.push_back((i * 2 + 2) % (stride * 2));
		m_indices.push_back((i * 2 + 3) % (stride * 2));
	}

}

void Cylinder::Rotate(XMMATRIX rotation)
{
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_vertices[i].pos);
		XMVECTOR nPos = XMVector3TransformCoord(pos, rotation);
		XMStoreFloat3(&m_vertices[i].pos, nPos);

		XMVECTOR norm = XMLoadFloat3(&m_vertices[i].normal);
		XMVECTOR nNorm = XMVector3TransformCoord(norm, rotation);
		XMStoreFloat3(&m_vertices[i].normal, nNorm);

		XMVECTOR tex = XMLoadFloat2(&m_vertices[i].tex);
		XMVECTOR nTex = XMVector2TransformCoord(tex, rotation);
		XMStoreFloat2(&m_vertices[i].tex, nTex);
	}
}

void Cylinder::Translate(XMVECTOR dir)
{
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_vertices[i].pos);
		XMVECTOR nPos = XMVectorAdd(pos, dir);
		XMStoreFloat3(&m_vertices[i].pos, nPos);
	}
}

XMFLOAT3 Cylinder::GetPosition(int i)
{
	return m_vertices[i].pos;
}

XMFLOAT2 Cylinder::GetTexCoord(int i)
{
	return m_vertices[i].tex;
}

XMFLOAT3 Cylinder::GetNormal(int i)
{
	return m_vertices[i].normal;
}

int Cylinder::GetNumVertices()
{
	return m_vertices.size();
}

std::vector<int>& Cylinder::GetIndices()
{
	return m_indices;
}