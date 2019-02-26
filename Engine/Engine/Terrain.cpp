
#include "Terrain.h"

Terrain::Terrain(int rows, int columns)
{
	m_height = rows;
	m_width = columns;

	m_vBuffer = 0;
	m_iBuffer = 0;
	m_toggleGenerated = false;

	m_vertices.reserve(rows * columns);
	VertexType vert;

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			//i is rows which is the "depth" ie z
			//j is the columns which is the width ie x
			vert.pos = XMFLOAT3(j, 0, i);
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
	iBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
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
}


void Terrain::CalcNormals()
{
	std::vector<std::vector<std::pair<XMFLOAT3, XMFLOAT3>>> faces;


}


int Terrain::GetIndexCount()
{
	return m_iCount;
}

