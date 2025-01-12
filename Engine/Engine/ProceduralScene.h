#pragma once

#include "L-System.h"
#include "LTree.h"
#include "Quad.h"
#include "Scene.h"
#include "Terrain.h"
#include "Text.h"

#include <array>
#include <string>
#include <time.h>

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
	bool RenderToTexture(D3D*, Camera*, ID3D11RenderTargetView*);
	bool RenderScene(D3D*, Camera*);

	void UpdateLight();

	bool IsCollidingTree(XMFLOAT3);


private:
	Terrain* m_terrain;
	LSystem* m_lsystem;
	std::array<LTree*, 3> m_trees;
	Model* m_sphere;
	Model* m_skySphere;
	Model* m_tree;

	Text* m_obj;
	std::string m_output;

	ID3D11Texture2D* m_ppRenderTarget;
	ID3D11RenderTargetView* m_ppRTV;
	ID3D11ShaderResourceView* m_ppSRV;
	XMMATRIX m_orthoProj;
	XMMATRIX m_ppView;
	Quad* m_ppQuad;

	Camera* m_mapCam;
	ID3D11Texture2D* m_mapRenderTarget;
	ID3D11ShaderResourceView* m_MapSRV;
	ID3D11RenderTargetView* m_mapRTV;
	XMMATRIX m_mapOrtho;
	XMMATRIX m_mapView;
	Quad* m_mapQuad;

	D3D11_VIEWPORT m_viewport;

	bool m_isInitialised;
	bool m_isSphereAlive;
	int m_sphereCount;
	float m_lightFactor;
};