#pragma once

#include "Scene.h"

class ProceduralScene : public Scene
{
public:
	ProceduralScene(ResourceManager::ManagerScene = ResourceManager::PROCEDURAL);
	~ProceduralScene();

	virtual bool Initialise(ID3D11Device*, ID3D11DeviceContext*) final;
	virtual bool Render(D3D*) final;

private:
	//Stuff might be needed here
};