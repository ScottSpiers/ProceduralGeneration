
#include "ProceduralScene.h"

ProceduralScene::ProceduralScene(ResourceManager::ManagerScene sceneResources) : Scene(sceneResources)
{
	m_terrain = 0;
}

ProceduralScene::~ProceduralScene()
{
	if (m_terrain)
	{
		delete m_terrain;
		m_terrain = 0;
	}
}

bool ProceduralScene::Initialise(ID3D11Device* device , ID3D11DeviceContext* context)
{
	bool result;

	result = Scene::Initialise(device, context);
	if (!result)
		return false;

	m_terrain = new Terrain(64, 64);
	//m_terrain->GenRandom();
	m_terrain->GenSinWave();

	m_Light->SetAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	//m_terrain->Initialise(device);
	m_Camera->SetPosition(50.0f, 2.0f, -7.0f);
	
}

bool ProceduralScene::Render(D3D* d3d)
{
	XMMATRIX viewMatrix, projMatrix;
	bool result;

	viewMatrix = m_Camera->GetViewMatrix();

	// Set the initial position of the camera.
	
	m_Camera->Render();

	
	d3d->GetProjectionMatrix(projMatrix);
	m_Camera->setProjMatrix(projMatrix);

	d3d->BeginScene(0.0f, 0.0f, 0.1f, 1.0f);
	//m_Camera->Render();

	/*d3d->GetDeviceContext()->ClearRenderTargetView(defRTV, colour);
	d3d->GetDeviceContext()->ClearDepthStencilView(defDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);*/

	//d3d->TurnOffCulling();
	
	

	result = m_terrain->Initialise(d3d->GetDevice());
	if (!result)
		return false;

	d3d->TurnOffCulling();

	result = m_terrain->Render(d3d->GetDeviceContext());
	if (!result)
		return false;

	result = m_shaders->RenderTerrain(m_terrain, m_Camera, m_Light);
	if (!result)
		return false;

	d3d->TurnOnCulling();

	//Model* orbitSphere = m_resources->GetModel(ResourceManager::ORBIT_MODEL);
	////orbitSphere->SetWorldMatrix(XMMatrixTranslation(0.f, 0.f, -.f));

	//orbitSphere->Render(d3d->GetDeviceContext());

	//result = m_shaders->RenderLight(orbitSphere, m_Camera, m_Light);
	//if (!result)
	//{
	//	return false;
	//}

	return true;
}