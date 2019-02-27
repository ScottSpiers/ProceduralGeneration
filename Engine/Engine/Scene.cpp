
#include "Scene.h"

Scene::Scene(ResourceManager::ManagerScene scene)
{
	m_Camera = 0;
	m_Light = 0;
	m_resources = 0;
	m_shaders = 0;
	m_managerType = scene;
}

Scene::~Scene()
{
	if (m_shaders)
	{
		delete m_shaders;
		m_shaders = 0;
	}

	if (m_resources)
	{
		delete m_resources;
		m_resources = 0;
	}

	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
}

bool Scene::Initialise(ID3D11Device* device, ID3D11DeviceContext* context)
{
	// Create the camera object.
	m_Camera = new Camera;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();

	// Create the light object.
	m_Light = new Light;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object with default values.
	m_Light->SetDiffuseColor(.9f, .9f, .9f, 1.f);
	m_Light->SetAmbientColour(0.85f, .85f, .85f, 1.f);
	m_Light->SetDirection(.75f, .0f, 1.0f);
	m_Light->SetSpecIntensity(20.0f);
	m_Light->SetSpecColour(1.0f, 1.0f, 1.0f, 1.0f);
	
	m_resources = new ResourceManager(device, context, m_managerType); 
	if (!m_resources->LoadResources())
		return false;

	m_shaders = new ShaderManager(device, context);
	if (!m_shaders->InitialiseShaders())
		return false;

	return true;
}

XMMATRIX Scene::GetView() const
{
	return m_Camera->GetViewMatrix();
}

void Scene::MoveCamera(XMFLOAT3 movement)
{
	m_Camera->Move(movement);
}

void Scene::RotateCamera(XMFLOAT3 rotation)
{
	m_Camera->Rotate(rotation);
}
