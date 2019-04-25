////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Model.h"


Model::Model()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_model = 0;
	m_textures = new ID3D11ShaderResourceView*[2];
	m_worldMatrix = XMMatrixIdentity();
}

Model::Model(Cylinder& c) : Model()
{
	m_worldMatrix = XMMatrixTranslationFromVector(XMVectorSet(c.GetPosition(0).x, c.GetPosition(0).y, c.GetPosition(0).z, 1.0f));
	m_model = new ModelData[c.GetNumVertices()];
	for (int i = 0; i < c.GetNumVertices(); ++i)
	{
		m_model[i].x = c.GetPosition(i).x;
		m_model[i].y = c.GetPosition(i).y;
		m_model[i].z = c.GetPosition(i).z;
		m_model[i].tu = c.GetTexCoord(i).x;
		m_model[i].tv = c.GetTexCoord(i).y;
		m_model[i].nx = c.GetNormal(i).x;
		m_model[i].ny = c.GetNormal(i).y;
		m_model[i].nz = c.GetNormal(i).z;
	}

	m_vertexCount = c.GetNumVertices();
	m_indices = c.GetIndices();
	m_indexCount = m_indices.size();
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
	int elems = sizeof(m_textures) / sizeof(m_textures[0]);

	for (int i = 0; i < elems; ++i)
	{
		if (m_textures[i])
		{
			m_textures[i]->Release();
			m_textures[i] = 0;
		}
	}
	delete[] m_textures;

	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

 void Model::drawCurrent(ID3D11DeviceContext* context) const
{
	 Render(context);
}


void Model::Render(ID3D11DeviceContext* deviceContext) const
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


int Model::GetIndexCount()
{
	return m_indexCount;
}


bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	//unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	//indices = new unsigned long[m_indexCount];
	/*if(!indices)
	{
		return false;
	}*/

	// Load the vertex array and index array with data.
	for(i = 0; i < m_vertexCount; ++i)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);

		m_indices.push_back(i);
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = m_indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	m_indices.clear();

	return true;
}

void Model::CalcVectors()
{
	int faceCount = m_vertexCount / 3;
	int index = 0;

	TempVertex v1, v2, v3;
	XMFLOAT3 tangent, binormal;
	XMFLOAT3 normal;

	for (int i = 0; i < faceCount; ++i)
	{
		v1.pos.x = m_model[index].x;
		v1.pos.y = m_model[index].y;
		v1.pos.z = m_model[index].z;
		v1.tex.x = m_model[index].tu;
		v1.tex.y = m_model[index].ty;
		v1.normal.x = m_model[index].nx;
		v1.normal.y = m_model[index].ny;
		v1.normal.z = m_model[index].nz;
		++index;

		v2.pos.x = m_model[index].x;
		v2.pos.y = m_model[index].y;
		v2.pos.z = m_model[index].z;
		v2.tex.x = m_model[index].tu;
		v2.tex.y = m_model[index].ty;
		v2.normal.x = m_model[index].nx;
		v2.normal.y = m_model[index].ny;
		v2.normal.z = m_model[index].nz;
		++index;

		v3.pos.x = m_model[index].x;
		v3.pos.y = m_model[index].y;
		v3.pos.z = m_model[index].z;
		v3.tex.x = m_model[index].tu;
		v3.tex.y = m_model[index].ty;
		v3.normal.x = m_model[index].nx;
		v3.normal.y = m_model[index].ny;
		v3.normal.z = m_model[index].nz;

		CalcTanBi(v1, v2, v3, tangent, binormal);
		normal = CalcNormal(tangent, binormal);

		m_model[index].nx = normal.x;
		m_model[index].ny = normal.y;
		m_model[index].nz = normal.z;
		m_model[index].tx = tangent.x;
		m_model[index].ty = tangent.y;
		m_model[index].tz = tangent.z;
		m_model[index].bx = binormal.x;
		m_model[index].by = binormal.y;
		m_model[index].bz = binormal.z;
		--index;

		m_model[index].nx = normal.x;
		m_model[index].ny = normal.y;
		m_model[index].nz = normal.z;
		m_model[index].tx = tangent.x;
		m_model[index].ty = tangent.y;
		m_model[index].tz = tangent.z;
		m_model[index].bx = binormal.x;
		m_model[index].by = binormal.y;
		m_model[index].bz = binormal.z;
		--index;

		m_model[index].nx = normal.x;
		m_model[index].ny = normal.y;
		m_model[index].nz = normal.z;
		m_model[index].tx = tangent.x;
		m_model[index].ty = tangent.y;
		m_model[index].tz = tangent.z;
		m_model[index].bx = binormal.x;
		m_model[index].by = binormal.y;
		m_model[index].bz = binormal.z;

		index += 3;
	}
}

//Wanted to split these but don't want to recalc vectors
void Model::CalcTanBi(TempVertex v1, TempVertex v2, TempVertex v3, XMFLOAT3& tan, XMFLOAT3& bi)
{
	XMVECTOR vec1, vec2, tVec1, tVec2;
	float den, length;

	vec1 = XMVectorSubtract(XMLoadFloat3(&v2.pos), XMLoadFloat3(&v1.pos));
	vec2 = XMVectorSubtract(XMLoadFloat3(&v3.pos), XMLoadFloat3(&v1.pos));

	XMFLOAT2 tuVec{ v2.tex.x - v1.tex.x, v3.tex.x - v1.tex.x };
	XMFLOAT2 tvVec{ v2.tex.y - v1.tex.y, v3.tex.y - v1.tex.y };

	den = 1.0f / ((tuVec.x * tvVec.y) - (tuVec.y * tvVec.x));

	XMFLOAT3 vec1f, vec2f, vec3f;
	XMStoreFloat3(&vec1f, vec1);
	XMStoreFloat3(&vec2f, vec2);

	tan.x = ((tvVec.y * vec1f.x) - (tvVec.x * vec2f.x)) * den;
	tan.y = ((tvVec.y * vec1f.y) - (tvVec.x * vec2f.y)) * den;
	tan.z = ((tvVec.y * vec1f.z) - (tvVec.x * vec2f.z)) * den;

	bi.x = ((tuVec.x * vec2f.x) - (tuVec.y * vec1f.x)) * den;
	bi.y = ((tuVec.x * vec2f.y) - (tuVec.y * vec1f.y)) * den;
	bi.z = ((tuVec.x * vec2f.z) - (tuVec.y * vec1f.z)) * den;

	length = sqrt((tan.x * tan.x) + (tan.y * tan.y) + (tan.z * tan.z));

	tan.x /= length;
	tan.y /= length;
	tan.z /= length;

	length = sqrt((bi.x * bi.x) + (bi.y * bi.y) + (bi.z * bi.z));

	bi.x /= length;
	bi.y /= length;
	bi.z /= length;
}

XMFLOAT3 Model::CalcNormal(XMFLOAT3 tan, XMFLOAT3 bi)
{
	float length;
	XMFLOAT3 norm;

	norm.x = (tan.y * bi.z) - (tan.z * bi.y);
	norm.y = (tan.z * bi.x) - (tan.x * bi.z);
	norm.z = (tan.x * bi.y) - (tan.y * bi.x);

	length = length = sqrt((norm.x * norm.x) + (norm.y * norm.y) + (norm.z * norm.z));

	norm.x /= length;
	norm.y /= length;
	norm.z /= length;

	return norm;
}

void Model::SetWorldMatrix(XMMATRIX world)
{
	m_worldMatrix = world;
}

XMMATRIX Model::GetWorldMatrix()
{
	return m_worldMatrix;
}

void Model::SetModelData(ModelData* data)
{
	m_model = data;
}

void Model::SetVertexCount(int count)
{
	m_vertexCount = count;
}

void Model::SetIndexCount(int count)
{
	m_indexCount = count;
}

void Model::SetTextures(ID3D11ShaderResourceView** textures)
{
	m_textures = textures;
}
ID3D11ShaderResourceView** Model::GetTextures()
{
	return m_textures;
}

