#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <stack>
#include <vector>

#include "Model.h"


using namespace DirectX;

class LTree
{
private:
	struct TurtleState
	{
		XMVECTOR pos;
		XMVECTOR dir = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
		float stepSize;
		float radius;
		XMMATRIX rotation;
	};

	//put this in a header! I'm using it everywhere
	struct VertexType
	{
		VertexType() {};
		VertexType(XMFLOAT3 position) : pos(position) {};
		//g_XMIdentityR1 is this the up vector?
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	LTree();
	LTree(float);
	~LTree();

	Model* InterpretSystem(std::string, float, float);

	void SetRadius(float);
	float GetRadius();
	Model* GetModel();

	void SetWorldMatrix(XMMATRIX);
	XMMATRIX GetWorldMatrix();
	int GetIndexCount();

	bool IsModel();

private:
	std::vector<VertexType> m_vertices;
	std::vector<int> m_indices;
	std::vector<Model*> m_models;
	Model* m_bigModel;

	XMMATRIX m_worldMatrix;
	bool m_isModel;
	float m_radius;
};