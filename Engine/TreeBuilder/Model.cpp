////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Model.h"


Model::Model()
{
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

void Model::WriteToFile(std::string filename)
{
	std::ofstream output;
	output.open(filename);

	output << "Vertex Count: " << m_vertices.size() << "\n\n";
	output << "Data:\n\n";
	
	
	for (VertexType v : m_vertices)
	{
		output << v.position.x << " " << v.position.y << " " << v.position.z << " ";
		output << v.texture.x << " " << v.texture.y << " ";
		output << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
	}

	output << "\nIndex Count: " << m_indices.size() << "\n\n";
	output << "Data:\n\n";

	for (unsigned int ind : m_indices)
	{
		output << ind << "\n";
	}


	output.close();

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

