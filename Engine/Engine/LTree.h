#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <stack>
#include <vector>

#include "LightShader.h"
#include "Model.h"
#include "ResourceManager.h"


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

	bool Initialise(ID3D11Device*);
	bool Render(ID3D11DeviceContext*);
	void InterpretSystem(std::string, float, float);


	void SetRadius(float);
	float GetRadius();
	Model* GetModel();

	void SetWorldMatrix(XMMATRIX);
	XMMATRIX GetWorldMatrix();
	int GetIndexCount();

	bool IsModel();
	void SetTextures(ID3D11ShaderResourceView**);
	//ID3D11ShaderResourceView* GetTexture();

private:
	int ParseParameters(std::string);
	void ApplyArithmetic(std::string&);

private:
	ID3D11Buffer* m_vBuffer, *m_iBuffer;
	ID3D11ShaderResourceView** m_textures;

	std::vector<VertexType> m_vertices;
	std::vector<int> m_indices;
	std::vector<Model*> m_models;
	Model* m_bigModel;

	XMMATRIX m_worldMatrix;
	bool m_isModel;
	float m_radius;
};