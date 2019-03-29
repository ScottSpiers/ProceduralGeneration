
#include "Cylinder.h"

Cylinder::Cylinder()
{

}

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
	float angle = (i * XM_2PI) / t;
	float deltaX;
	float deltaZ;

	XMScalarSinCos(&deltaX, &deltaZ, angle);
	
	return XMVectorSet(deltaX, 0, deltaZ, 0.f);
}

void Cylinder::GenCaps(float r, float h, int t, bool isTop)
{
	for (int i = 0; i < t - 2; ++i)
	{
		int index1 = (i + 1) % t;
		int index2 = (i + 2) % t;

		if (isTop)
		{
			int temp = index1;
			index1 = index2;
			index2 = temp;
		}

		int verts = m_vertices.size();
		m_indices.push_back(verts);
		m_indices.push_back(verts + index1);
		m_indices.push_back(verts + index2);
	}

	XMVECTOR normal = g_XMIdentityR1; //UP
	XMVECTOR texScale = g_XMNegativeOneHalf;

	if (!isTop)
	{
		normal = XMVectorNegate(normal);
		texScale = XMVectorMultiply(texScale, g_XMNegateX);
	}

	for (int i = 0; i < t; ++i)
	{
		XMVECTOR circVec = GetCircleVector(i, t);
		XMVECTOR pos = XMVectorAdd(XMVectorScale(circVec, r), XMVectorScale(normal, h));
		XMVECTOR texCoord = XMVectorMultiplyAdd(XMVectorSwizzle<0, 2, 3, 3>(circVec), texScale, g_XMOneHalf);
		Vertex v;
		XMStoreFloat3(&v.pos, pos);
		XMStoreFloat3(&v.normal, normal);
		XMStoreFloat2(&v.tex, texCoord);
		m_vertices.push_back(v);
	}
}

void Cylinder::GenCylinder(float r, float h, int t)
{
	m_vertices.clear();
	m_indices.clear();

	float height = float(h) / 2;

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
		m_vertices.push_back(Vertex(v.pos, v.normal, v.tex));

		//m_vertices[i].pos = XMFLOAT3(m_vertices[i].normal.x * r, i / t , m_vertices[i].normal.z * r);

		XMStoreFloat3(&v.pos, XMVectorSubtract(offsetSide, offsetTop));
		XMStoreFloat3(&v.normal, normal);
		XMStoreFloat2(&v.tex, XMVectorAdd(texCoord, g_XMIdentityR1));
		m_vertices.push_back(Vertex(v.pos, v.normal, v.tex));
		
		//fully stolen from the link above
		m_indices.push_back(i * 2 + 1);
		m_indices.push_back((i * 2 + 2) % (stride * 2));
		m_indices.push_back(i * 2);

		m_indices.push_back((i * 2 + 3) % (stride * 2));
		m_indices.push_back((i * 2 + 2) % (stride * 2));
		m_indices.push_back(i * 2 + 1);
	}

	GenCaps(r, height, t, true);
	GenCaps(r, height, t, false);

	for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		it->tex.x = (1.f - it->tex.x);
	}
}

void Cylinder::Rotate(XMMATRIX rotation)
{
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_vertices[i].pos);
		XMVECTOR nPos = XMVector3Transform(pos, rotation);
		XMStoreFloat3(&m_vertices[i].pos, nPos);

		XMVECTOR norm = XMLoadFloat3(&m_vertices[i].normal);
		XMVECTOR nNorm = XMVector3TransformNormal(norm, rotation);
		XMStoreFloat3(&m_vertices[i].normal, nNorm);

		XMVECTOR tex = XMLoadFloat2(&m_vertices[i].tex);
		XMVECTOR nTex = XMVector2Transform(tex, rotation);
		XMStoreFloat2(&m_vertices[i].tex, nTex);		
	}
}

void Cylinder::Translate(XMVECTOR dir)
{
	XMMATRIX trans = XMMatrixTranslationFromVector(dir);
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		XMVECTOR pos = XMLoadFloat3(&m_vertices[i].pos);
		XMVECTOR nPos = XMVector3Transform(pos, trans);
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

std::vector<int> Cylinder::GetIndices()
{
	return m_indices;
}