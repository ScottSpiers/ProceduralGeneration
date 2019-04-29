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

	std::vector<VertexType> GetVertices();
	std::vector<unsigned int> GetIndices();
	int GetIndexCount();
	int GetVertexCount();

	void SetWorldMatrix(XMMATRIX);
	XMMATRIX GetWorldMatrix();

	void SetModelData(std::vector<VertexType>&);
	void SetModelData(std::vector<VertexType>&, std::vector<unsigned int>&);

	void WriteToFile(std::string);

private:
	std::vector<VertexType> m_vertices;
	std::vector<unsigned int> m_indices;

	XMMATRIX m_worldMatrix;
};

#endif