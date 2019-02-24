#pragma once

#include <directxmath.h>
#include <memory>
#include <vector>

using namespace DirectX;

class SceneNode 
{

public:
	typedef std::unique_ptr<SceneNode> uSceneNode;

public:
	SceneNode();
	~SceneNode();

	void addChild(uSceneNode);
	uSceneNode deleteChild(const SceneNode&);

	bool IsEnabled();

private:
	virtual void draw() const final;
	virtual void drawCurrent() const;


protected:
	SceneNode* m_parent;
	std::vector<uSceneNode> m_children;

	XMMATRIX m_worldMtrix;

	bool m_isEnabled;
};