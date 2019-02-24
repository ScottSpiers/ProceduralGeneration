
#include "SceneNode.h"

SceneNode::SceneNode() : m_children()
{
	m_parent = 0;
	m_worldMatrix = XMMatrixIdentity();
}

SceneNode::SceneNode(SceneNode* parent) : SceneNode()
{
	m_parent = parent;
	parent->addChild(uSceneNode(this));
	m_worldMatrix = parent->GetWorldMatrix();
}

SceneNode::~SceneNode()
{
	if (m_parent)
	{
		delete m_parent;
		m_parent = 0;
	}
}

void SceneNode::addChild(uSceneNode child)
{
	child->m_parent = this;
	m_children.push_back(std::move(child));

}

SceneNode::uSceneNode SceneNode::deleteChild(const SceneNode& node)
{
	for (auto itr = m_children.begin(); itr != m_children.end(); ++itr)
	{
		if (itr->get() == &node)
		{
			uSceneNode res = std::move(*itr);
			res->m_parent = nullptr;
			m_children.erase(itr);
			return res;
		}
	}
}

bool SceneNode::IsEnabled() const
{
	return m_isEnabled;
}

void SceneNode::SetEnabled(bool enabled)
{
	m_isEnabled = enabled;
}

void SceneNode::draw(ID3D11DeviceContext* context) const
{
	if (m_isEnabled)
	{
		drawCurrent(context);

		for (auto itr = m_children.begin(); itr != m_children.end(); ++itr)
			(*itr)->draw(context);
	}
}

void SceneNode::drawCurrent(ID3D11DeviceContext* context) const
{
	//Do Nothing By Default
}

XMMATRIX SceneNode::GetWorldMatrix() const
{
	return m_worldMatrix;
}

void SceneNode::SetWorldMatrix(XMMATRIX world)
{
	m_worldMatrix = world;
}

