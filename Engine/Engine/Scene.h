#pragma once

#include <d3d11.h>


#include "Camera.h"
#include "D3D.h"
#include "Light.h"
#include "Model.h"
#include "ResourceManager.h"
#include "ShaderManager.h"

//#include "SceneNode.h"
//#include <array>


class Scene
{

public:
	Scene(ResourceManager::ManagerScene);
	~Scene();

	virtual bool Initialise(ID3D11Device*, ID3D11DeviceContext*);
	virtual bool Render(D3D*) = 0;
	//bool Update(); //add once we have time stuff working

	void MoveCamera(XMFLOAT3);
	void RotateCamera(XMFLOAT3);

	//Think of a better way to handle this stuff
	XMMATRIX GetView();

private:
	/*enum Layer 
	{
		BACKGROUND,
		FOREGROUND,
		SKYSPHERE,
		MAX_LAYER_COUNT
	};*/

protected:
	Camera* m_Camera;
	Light* m_Light;

	ResourceManager* m_resources;
	ResourceManager::ManagerScene m_managerType;

	ShaderManager* m_shaders;

	/*SceneNode m_sceneGraph;
	std::array<SceneNode*, MAX_LAYER_COUNT> m_sceneLayers;*/
};