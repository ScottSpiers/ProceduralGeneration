////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Model.h"


Model::Model()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_textures = new ID3D11ShaderResourceView*[2];
	m_worldMatrix = XMMatrixIdentity();
}

Model::Model(Cylinder& c) : Model()
{
	m_worldMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&c.GetPosition(0)));
	m_vertices.resize(c.GetNumVertices());

	for (int i = 0; i < c.GetNumVertices(); ++i)
	{
		m_vertices[i].position = c.GetPosition(i);
		m_vertices[i].texture = c.GetTexCoord(i);
		m_vertices[i].normal = c.GetNormal(i);
	}

	m_indices = c.GetIndices();
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

bool Model::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//CalcVectors(); //THis is acting weird
	// Load the vertex array and index array with data.
	for(i = 0; i < m_vertices.size(); ++i)
	{
		m_indices.push_back(i);
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices.data();
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
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indices.size();
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

	return true;
}

void Model::CalcVectors()
{
	int faceCount = m_vertices.size() / 3;
	int index = 0;

	TempVertex v1, v2, v3;
	XMFLOAT3 tangent, binormal;
	XMFLOAT3 normal;

	for (int i = 0; i < faceCount; ++i)
	{
		v1.pos  = m_vertices[index].position;
		v1.tex = m_vertices[index].texture;
		v1.normal = m_vertices[index].normal;
		++index;

		v2.pos = m_vertices[index].position;
		v2.tex = m_vertices[index].texture;
		v2.normal = m_vertices[index].normal;
		++index;

		v3.pos = m_vertices[index].position;
		v3.tex = m_vertices[index].texture;
		v3.normal = m_vertices[index].normal;

		CalcTanBi(v1, v2, v3, tangent, binormal);
		normal = CalcNormal(tangent, binormal);

		m_vertices[index].normal = normal;
		m_vertices[index].tangent = tangent;
		m_vertices[index].binormal = binormal;
		--index;

		m_vertices[index].normal = normal;
		m_vertices[index].tangent = tangent;
		m_vertices[index].binormal = binormal;
		--index;

		m_vertices[index].normal = normal;
		m_vertices[index].tangent = tangent;
		m_vertices[index].binormal = binormal;

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

void Model::SetModelData(std::vector<VertexType>& vertices, std::vector<unsigned int>& indices)
{
	m_vertices = vertices;
	m_indices = indices;
}

void Model::SetModelData(std::vector<VertexType>& data)
{
	m_vertices = data;
}

std::vector<Model::VertexType> Model::GetVertices()
{
	return m_vertices;
}

std::vector<unsigned int> Model::GetIndices()
{
	return m_indices;
}

int Model::GetIndexCount()
{
	return m_indices.size();
}

int Model::GetVertexCount()
{
	return m_vertices.size();
}

void Model::SetTextures(ID3D11ShaderResourceView** textures)
{
	m_textures = textures;
}
ID3D11ShaderResourceView** Model::GetTextures()
{
	return m_textures;
}

