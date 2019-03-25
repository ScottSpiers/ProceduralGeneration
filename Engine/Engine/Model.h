////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////

#include <DirectXMath.h>
#include <d3d11.h>
#include <fstream>

#include "Cylinder.h"
//#include "SceneNode.h"

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class Model
{

public:
	struct ModelData
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

protected:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
		XMFLOAT3 normal;
	};	

public:
	Model();
	Model(Cylinder);
	Model(const Model&);
	~Model();

	bool InitializeBuffers(ID3D11Device*);
	void Render(ID3D11DeviceContext*) const;

	int GetIndexCount();

	void SetWorldMatrix(XMMATRIX);
	XMMATRIX GetWorldMatrix();

	void SetTexture(ID3D11ShaderResourceView*);
	ID3D11ShaderResourceView* GetTexture();

	void SetModelData(ModelData*);
	void SetVertexCount(int);
	void SetIndexCount(int);

protected:
	virtual void drawCurrent(ID3D11DeviceContext*) const;

private:
	void ReleaseBuffers();

	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelData* m_model;
	std::vector<int> m_indices;

	XMMATRIX m_worldMatrix;
	ID3D11ShaderResourceView* m_texture;
};

#endif