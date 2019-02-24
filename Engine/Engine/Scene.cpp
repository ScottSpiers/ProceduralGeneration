
#include "Scene.h"

Scene::Scene() : m_sceneGraph(), m_sceneLayers()
{
}

Scene::~Scene()
{
}

void Scene::Render()
{
	//TODO: Copy Render code from graphics here after updated to handle scene graph
	m_sceneGraph.draw();
	
}