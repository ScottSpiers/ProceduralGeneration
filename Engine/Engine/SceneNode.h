//#pragma once
//
//#include <d3d11.h>
//#include <directxmath.h>
//#include <memory>
//#include <vector>
//
//using namespace DirectX;
//
//class SceneNode 
//{
//
//public:
//	typedef std::unique_ptr<SceneNode> uSceneNode;
//
//public:
//	SceneNode();
//	SceneNode(SceneNode*);
//	~SceneNode();
//
//	void addChild(uSceneNode);
//	uSceneNode deleteChild(const SceneNode&);
//
//	virtual void draw(ID3D11DeviceContext*) const final;
//
//	XMMATRIX GetWorldMatrix() const;
//	void SetWorldMatrix(XMMATRIX);
//	
//	bool IsEnabled() const;
//	void SetEnabled(bool enabled);
//
//protected:
//	virtual void drawCurrent(ID3D11DeviceContext*) const;
//
//
//protected:
//	SceneNode* m_parent;
//	std::vector<uSceneNode> m_children;
//
//	XMMATRIX m_worldMatrix;
//
//	bool m_isEnabled;
//};