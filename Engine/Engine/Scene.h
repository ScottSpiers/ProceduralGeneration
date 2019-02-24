#pragma once

#include "SceneNode.h"
#include <array>


class Scene
{

public:

	Scene();
	~Scene();

	virtual void Render();
	//bool Update(); //add once we have time stuff working

private:
	enum Layer 
	{
		BACKGROUND,
		FOREGROUND,
		SKYSPHERE,
		MAX_LAYER_COUNT
	};

protected:
	SceneNode m_sceneGraph;
	std::array<SceneNode*, MAX_LAYER_COUNT> m_sceneLayers;
};