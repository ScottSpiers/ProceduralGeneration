#pragma once

#include "L-System.h"
#include "LTree.h"
#include "Scene.h"
#include "Terrain.h"

#include <array>

class ProceduralScene : public Scene
{
public:
	ProceduralScene(ResourceManager::ManagerScene = ResourceManager::PROCEDURAL);
	~ProceduralScene();

	virtual bool Initialise(ID3D11Device*, ID3D11DeviceContext*) final;
	virtual bool Render(D3D*) final;

private:
	Terrain* m_terrain;
	LSystem* m_lsystem;
	std::array<LTree*, 5> m_trees;
};