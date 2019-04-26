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
		float tx, ty, tz;
		float bx, by, bz;
	};
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};	

private:
	struct TempVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	Model();
	Model(Cylinder&);
	Model(const Model&);
	~Model();

	bool InitializeBuffers(ID3D11Device*);
	void Render(ID3D11DeviceContext*) const;

	std::vector<VertexType> GetVertices();
	std::vector<unsigned int> GetIndices();
	int GetIndexCount();
	int GetVertexCount();

	void SetWorldMatrix(XMMATRIX);
	XMMATRIX GetWorldMatrix();

	void SetTextures(ID3D11ShaderResourceView**);
	ID3D11ShaderResourceView** GetTextures();

	void SetModelData(std::vector<VertexType>&);
	void SetModelData(std::vector<VertexType>&, std::vector<unsigned int>&);

protected:
	virtual void drawCurrent(ID3D11DeviceContext*) const;

private:
	/*void CalcVectors();
	void CalcTanBi(TempVertex, TempVertex, TempVertex, XMFLOAT3&, XMFLOAT3&);
	XMFLOAT3 CalcNormal(XMFLOAT3, XMFLOAT3);*/

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	std::vector<VertexType> m_vertices;
	std::vector<unsigned int> m_indices;

	XMMATRIX m_worldMatrix;
	ID3D11ShaderResourceView** m_textures;
};

#endif