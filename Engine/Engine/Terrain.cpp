
#include "Terrain.h"


Terrain::Terrain(int rows, int columns)
{
	m_height = rows;
	m_width = columns;

	m_vBuffer = 0;
	m_iBuffer = 0;
	m_toggleGenerated = false;

	m_worldMatrix = XMMatrixIdentity();

	m_vertices.reserve(rows * columns);
	VertexType vert;

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			//i is rows which is the "depth" ie z
			//j is the columns which is the width ie x
			XMFLOAT3 pos = XMFLOAT3(j, 0.f, i);
			vert.pos = XMLoadFloat3(&pos);
			m_vertices.push_back(vert);
		}
	}
}

Terrain::~Terrain()
{
	if (m_iBuffer)
	{
		m_iBuffer->Release();
		m_iBuffer = 0;
	}

	if (m_vBuffer)
	{
		m_vBuffer->Release();
		m_vBuffer = 0;
	}

}

bool Terrain::Initialise(ID3D11Device* device)
{
	CalcNormals();

	D3D11_BUFFER_DESC vBufferDesc;
	D3D11_BUFFER_DESC iBufferDesc;

	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;

	HRESULT result;

	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(VertexType) * m_vertices.size();
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	vData.pSysMem = m_vertices.data();
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vBufferDesc, &vData, &m_vBuffer);
	if (FAILED(result))
		return false;

	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(int) * m_indices.size();
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	iData.pSysMem = m_indices.data();
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&iBufferDesc, &iData, &m_iBuffer);
	if (FAILED(result))
		return false;

	return true;
}


bool Terrain::Render(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}


void Terrain::CalcNormals()
{
	//Setup the terrain faces "map" .
	//Stores the face indices and the face normal
	std::vector<std::vector<std::pair<XMFLOAT3, XMFLOAT3>>> faces;
	faces.resize(m_height - 1);

	for (auto r : faces)
	{
		r.reserve((m_width - 1) * 2);
	}

	//Calculate indices for the square
	for (int r = 0; r < m_height - 1; ++r)
	{
		for (int c = 0; c < m_width - 1; ++c)
		{
			int topLeft = (r * m_width) + c;
			int topRight = topLeft + 1;
			int bottomLeft = ((r + 1) * m_width) + c;
			int bottomRight = bottomLeft + 1;

			//Calculate vectors for top left triangle face
			
			XMVECTOR ab = XMVectorSubtract(m_vertices[topLeft].pos, m_vertices[bottomLeft].pos);
			XMVECTOR ac = XMVectorSubtract(m_vertices[topRight].pos, m_vertices[bottomLeft].pos);
			//Calculate the normal
			XMVECTOR normal = XMVector3Cross(ac, ab);
			XMFLOAT3 n;
			XMStoreFloat3(&n, normal);
			//Store the face, normal pair
			faces[r].push_back(std::make_pair(XMFLOAT3(topLeft, bottomLeft, topRight), n));

			//Calculate normal for bottom right triangle face
			ab = XMVectorSubtract(m_vertices[bottomLeft].pos, m_vertices[bottomRight].pos);
			ac = XMVectorSubtract(m_vertices[topRight].pos, m_vertices[bottomRight].pos);
			normal = XMVector3Cross(ac, ab);
			XMStoreFloat3(&n, normal);
			faces[r].push_back(std::make_pair(XMFLOAT3(bottomLeft, bottomRight, topRight), n));
		}
	}

	//Calculate the vertex normals from the faces
	for (int r = 0; r < m_height; ++r)
	{
		std::vector<std::pair<XMFLOAT3, XMFLOAT3>> facesToCheck;

		if (r - 1 >= 0)
			facesToCheck = faces[r - 1];
		if(r != m_height -1)
			facesToCheck.insert(facesToCheck.end(), faces[r].begin(), faces[r].end());			

		for (int c = 0; c < m_width; ++c)
		{
			int index = (r * m_width) + c;
			int facesFound = 0;

			XMVECTOR normalSum;
			normalSum = XMVectorSet(.0f, .0f, .0f, .0f);
			XMVECTOR normalToAdd;

			for (auto f : facesToCheck)
			{
				if (index == f.first.x || index == f.first.y || index == f.first.z)
				{
					normalToAdd = XMLoadFloat3(&f.second);
					normalSum = XMVectorAdd(normalSum, normalToAdd);
					++facesFound;
				}

				//A vertex can be part of at most 6 faces
				if (facesFound == 6)
					break;
			}

			//normalSum /= facesFound;
			//Ask about this

			normalSum = XMVector3Normalize(normalSum);
			m_vertices[index].normal = normalSum;
		}
	}

	//Store the indices in order
	m_indices.clear();
	m_indices.reserve(m_height * m_width * 6);

	//go through the rows of faces
	for (auto row : faces)
	{
		//for every face in the current row
		for (auto face : row)
		{
			m_indices.push_back(face.first.x);
			m_indices.push_back(face.first.y);
			m_indices.push_back(face.first.z);
		}
	}
}

void Terrain::GenRandom()
{	
	srand(time(nullptr));
	if (!m_toggleGenerated)
	{
		int index;
		float height = 0.0;

		for (int i = 0; i < m_height; ++i)
		{
			for (int j = 0; j < m_width; ++j)
			{
				index = (i * m_width) + j;

				float r = (double)rand() / (RAND_MAX + 1);
				XMFLOAT3 pos = XMFLOAT3((float)j, r * 3, (float)i);

				m_vertices[index].pos = XMLoadFloat3(&pos);
			}
		}
		m_toggleGenerated = true;
	}
}

void Terrain::GenSinWave()
{
	if (!m_toggleGenerated)
	{

		int index;
		float height = 0.0;

		//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
		//in this case I will run a sin-wave through the terrain in one axis.

		for (int j = 0; j < m_height; j++)
		{
			for (int i = 0; i < m_width; i++)
			{
				index = (m_height * j) + i;

				XMFLOAT3 pos = XMFLOAT3((float)i, (float)(sin((float)i / (m_width / 12))*3.0) * (float)(cos((float)j / (m_height / 12))*3.0), (float)j);
				m_vertices[index].pos = XMLoadFloat3(&pos);
			}
		}
		m_toggleGenerated = true;
	}
}

XMMATRIX Terrain::GetWorldMatrix()
{
	return m_worldMatrix;
}

int Terrain::GetIndexCount()
{
	return m_indices.size();
}

