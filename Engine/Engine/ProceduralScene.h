#pragma once

#include "L-System.h"
#include "LTree.h"
#include "Quad.h"
#include "Scene.h"
#include "Terrain.h"

#include <array>

class ProceduralScene : public Scene
{
public:
	ProceduralScene();
	~ProceduralScene();

	virtual bool Initialise(ID3D11Device*, ID3D11DeviceContext*) final;
	virtual bool Render(D3D*) final;

	virtual void Modify(ID3D11Device*) final;

	virtual void MoveCamera(XMFLOAT3) final;

private:
	bool InitialiseRenderTexture(ID3D11Device*, XMFLOAT2);
	bool RenderToTexture(D3D*);
	bool RenderScene(D3D*);

	bool IsCollidingTree(XMFLOAT3);


private:
	Terrain* m_terrain;
	LSystem* m_lsystem;
	std::array<LTree*, 5> m_trees;
	Model* m_sphere;
	Model* m_skySphere;
	Model* m_tree;

	std::string m_output;

	ID3D11Texture2D* m_ppRenderTarget;
	ID3D11RenderTargetView* m_ppRTV;
	ID3D11ShaderResourceView* m_ppSRV;
	XMMATRIX m_orthoProj;
	XMMATRIX m_ppView;
	Quad* m_ppQuad;

	bool m_isSphereAlive;
	int m_sphereCount;
};