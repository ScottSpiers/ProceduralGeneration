
#include "Terrain.h"
#include "ImprovedNoise.h"

Terrain::Terrain(int rows, int columns)
{
	m_height = rows;
	m_width = columns;

	m_vBuffer = 0;
	m_iBuffer = 0;
	m_textures = 0;
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
			//vert.pos = XMLoadFloat3(&pos);
			vert.pos = pos;
			m_vertices.push_back(vert);
		}
	}
	srand(time(nullptr));
}

Terrain::~Terrain()
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
	CalcTexCoords();

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
	for (int row = 0; row < m_height - 1; ++row)
	{
		for (int col = 0; col < m_width - 1; ++col)
		{
			int topLeft = (row * m_width) + col;
			int topRight = topLeft + 1;
			int bottomLeft = ((row + 1) * m_width) + col;
			int bottomRight = bottomLeft + 1;

			//Calculate vectors for top left triangle face
			
			XMVECTOR a = XMLoadFloat3(&m_vertices[topLeft].pos);
			XMVECTOR b = XMLoadFloat3(&m_vertices[bottomLeft].pos);
			XMVECTOR c = XMLoadFloat3(&m_vertices[topRight].pos);

			XMVECTOR ab = XMVectorSubtract(a, b); //XMVectorSubtract(m_vertices[topLeft].pos, m_vertices[bottomLeft].pos);
			XMVECTOR cb = XMVectorSubtract(c, b); //XMVectorSubtract(m_vertices[topRight].pos, m_vertices[bottomLeft].pos);
			//Calculate the normal
			XMVECTOR normal = XMVector3Cross(cb, ab);
			XMFLOAT3 n;
			XMStoreFloat3(&n, normal);
			//Store the face, normal pair
			faces[row].push_back(std::make_pair(XMFLOAT3(topLeft, bottomLeft, topRight), n));

			a = XMLoadFloat3(&m_vertices[bottomLeft].pos);
			b = XMLoadFloat3(&m_vertices[bottomRight].pos);
			c = XMLoadFloat3(&m_vertices[topRight].pos);

			//Calculate normal for bottom right triangle face
			ab = XMVectorSubtract(a, b); //XMVectorSubtract(m_vertices[bottomLeft].pos, m_vertices[bottomRight].pos);
			cb = XMVectorSubtract(c, b); //XMVectorSubtract(m_vertices[topRight].pos, m_vertices[bottomRight].pos);

			normal = XMVector3Cross(cb, ab);
			XMStoreFloat3(&n, normal);
			faces[row].push_back(std::make_pair(XMFLOAT3(bottomLeft, bottomRight, topRight), n));
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
			XMStoreFloat3(&m_vertices[index].normal, normalSum);
			//m_vertices[index].normal = normalSum;
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

void Terrain::CalcTexCoords()
{
	float x = 32.0f / m_width;
	float z = 32.0f / m_height;

	for (auto& v : m_vertices)
	{
		v.tex = XMFLOAT2(v.pos.x * x, v.pos.z * z);
	}
}

void Terrain::SetTextures(ID3D11ShaderResourceView** t)
{
	m_textures = t;
}

ID3D11ShaderResourceView** Terrain::GetTextures()
{
	return m_textures;
}

float Terrain::GetTerrainHeight(float x, float z)
{
	bool inX = x < m_width - 1 && x >= 0;
	bool inZ = z < m_height -1 && z >= 0;

	if (!inX || !inZ)
		return 0.0f;

	/*x /= m_width;
	z /= m_height;*/

	int row = floorf(z);
	int col = floorf(x);

	float h = 0.0f;

	int index = (row * m_width) + col;
	float a = m_vertices[index].pos.y;
	float b = m_vertices[index + 1].pos.y;

	index = ((row + 1) * m_width) + col;
	float c = m_vertices[index].pos.y;
	float d = m_vertices[index + 1].pos.y;

	float deltaX = x - col;
	float deltaZ = z - row;

	if (deltaZ <= 1.0f - deltaX)
	{
		float uy = b - a;
		float vy = c - a;
		h = a + (deltaX * uy) + (deltaZ * vy);
	}
	else
	{
		float uy = c - d;
		float vy = b - d;
		h = d + ((1 - deltaX) * uy) + ((1 - deltaZ) * vy);
	}
	return h;
}

void Terrain::GenRandom()
{	
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

				//m_vertices[index].pos = XMLoadFloat3(&pos);
				m_vertices[index].pos = pos;
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
				//m_vertices[index].pos = XMLoadFloat3(&pos);
				m_vertices[index].pos = pos;
			}
		}
		m_toggleGenerated = true;
	}
}

void Terrain::GenPerlin()
{
	ImprovedNoise::perlin();
	if (!m_toggleGenerated)
	{
		int index;
		double height = 0.0f;
		int numOctaves = 8;

		
		for (int i = 0; i < m_height; ++i)
		{
			for (int j = 0; j < m_width; ++j)
			{
				double elevation = 0.0f;
				double amp = 1.0f;
				double st = 1.0f;

				index = (m_height * i) + j;
				double nx = m_vertices[index].pos.x * 10 * (1.0 / m_width);
				double ny = m_vertices[index].pos.y * 10;
				double nz = m_vertices[index].pos.z * 10 * (1.0 / m_height);

				for (int k = 0; k < numOctaves; ++k)
				{
					elevation += amp * ImprovedNoise::noise(st * nx, st * ny, st *nz);
					st *= 2;
					amp *= 0.5;
				}


				/*elevation += 0.5 * ImprovedNoise::noise(2 * nx, 2 * ny, 2 * nz);
				elevation += 0.25 * ImprovedNoise::noise(4 * nx, 4 * ny, 4 * nz);*/

				XMFLOAT3 pos = XMFLOAT3((float) j, 7* elevation ,(float)i);
				m_vertices[index].pos = pos;
			}
		}


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

