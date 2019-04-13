
#include "Quad.h"

Quad::Quad(int x, int y)
{
	m_vBuffer = 0;
	m_iBuffer = 0;
	m_height = y;
	m_width = x;
	m_worldMatrix = XMMatrixIdentity();
}

Quad::~Quad()
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

bool Quad::Initialise(ID3D11Device* device)
{
	float top, bottom, left, right;
	D3D11_BUFFER_DESC vbufferDesc, iBufferDesc;
	D3D11_SUBRESOURCE_DATA vData, iData;
	HRESULT res;

	left = (m_width / 2.0f) * (-1.0f);
	right = left + m_width;
	top = (m_height / 2.0f);
	bottom = top - m_height;

	Vertex v;
	v.pos = XMFLOAT3(left, top, 0.0f);
	v.tex = XMFLOAT2(0.0f, 0.0f);
	m_vertices.push_back(v);
	
	v.pos = XMFLOAT3(right, top, 0.0f);
	v.tex = XMFLOAT2(1.0f, 0.0f);
	m_vertices.push_back(v);

	v.pos = XMFLOAT3(left, bottom, 0.0f);
	v.tex = XMFLOAT2(0.0f, 1.0f);
	m_vertices.push_back(v);

	v.pos = XMFLOAT3(right, bottom, 0.0f);
	v.tex = XMFLOAT2(1.0f, 1.0f);
	m_vertices.push_back(v);


	m_indices.push_back(0);
	m_indices.push_back(2);
	m_indices.push_back(1);

	m_indices.push_back(1);
	m_indices.push_back(2);
	m_indices.push_back(3);

	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.ByteWidth = sizeof(Vertex) * m_vertices.size();
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.StructureByteStride = 0;

	vData.pSysMem = m_vertices.data();
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	res = device->CreateBuffer(&vbufferDesc, &vData, &m_vBuffer);
	if (FAILED(res))
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

	res = device->CreateBuffer(&iBufferDesc, &iData, &m_iBuffer);
	if (FAILED(res))
		return false;

	return true;
}

void Quad::Render(ID3D11DeviceContext* context)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	context->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32G32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Quad::SetTexture(ID3D11ShaderResourceView* t)
{
	m_texture = t;
}

ID3D11ShaderResourceView* Quad::GetTexture()
{
	return m_texture;
}

void Quad::SetWorldMatrix(XMMATRIX world)
{
	m_worldMatrix = world;
}

XMMATRIX Quad::GetWorldMatrix()
{
	return m_worldMatrix;
}

int Quad::GetHeight()
{
	return m_height;
}

int Quad::GetWidth()
{
	return m_width;
}

int Quad::GetIndexCount()
{
	return m_indices.size();
}

