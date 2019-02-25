
#include "WinterReflection.h"

WinterReflection::WinterReflection(ResourceManager::ManagerScene sceneResources) : Scene(sceneResources)
{
	m_cMap = 0;
	m_cMapSRV = 0;
	m_passingTime = 0.0f;
}

WinterReflection::~WinterReflection()
{
	if (m_cMapSRV)
	{
		m_cMapSRV->Release();
		m_cMapSRV = 0;
	}

	if (m_cMap)
	{
		m_cMap->Shutdown();
		delete m_cMap;
		m_cMap = 0;
	}
}

bool WinterReflection::Initialise(ID3D11Device* device, ID3D11DeviceContext* context)
{
	bool result;
	//Initialise the Default objects first
	result = Scene::Initialise(device, context);
	if (!result)
		return false;

	m_cMap = new Cubemap;
	if (!m_cMap)
		return false;

	//Initialise this scenes objects
	return true;
}

bool WinterReflection::Render(D3D* d3d)
{
	RenderCubemap(d3d);

	XMMATRIX projMatrix;
	d3d->GetProjectionMatrix(projMatrix);
	m_Camera->setProjMatrix(projMatrix);
	m_Camera->Render();
	return RenderScene(d3d, m_Camera, true);
}

bool WinterReflection::RenderCubemap(D3D* d3d)
{
	D3D11_VIEWPORT vport;
	UINT num_viewps = 1;
	ID3D11RenderTargetView* defRTV = d3d->getRenderTargetView();
	ID3D11DepthStencilView* defDSV = d3d->getDepthStencilView();
	ID3D11RenderTargetView* cMapRTV = 0;

	d3d->GetDeviceContext()->RSGetViewports(&(num_viewps), &vport);
	m_cMap->Initialise(d3d->GetDevice());

	d3d->BeginScene(0.0f, 0.0f, 1.0f, 1.0f);

	d3d->GetDeviceContext()->RSSetViewports(1, &(m_cMap->getViewport()));
	m_cMap->InitCameras(0.f, 0.f, 0.f);
	float colour[4]{ 1.f, 0.0f, 0.0f, 1.f };

	for (int i = 0; i < 6; ++i)
	{
		cMapRTV = m_cMap->getRTV(i);

		d3d->GetDeviceContext()->ClearRenderTargetView(cMapRTV, colour);
		d3d->GetDeviceContext()->ClearDepthStencilView(m_cMap->getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		d3d->GetDeviceContext()->OMSetRenderTargets(1, &(cMapRTV), m_cMap->getDSV());
		RenderScene(d3d, m_cMap->getView(i), false);
	}

	d3d->GetDeviceContext()->RSSetViewports(1, &vport);
	d3d->GetDeviceContext()->OMSetRenderTargets(1, &(defRTV), defDSV);
	d3d->GetDeviceContext()->GenerateMips(m_cMap->getSRV());

	d3d->GetDeviceContext()->ClearRenderTargetView(defRTV, colour);
	d3d->GetDeviceContext()->ClearDepthStencilView(defDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_cMapSRV = m_cMap->getSRV();
	

	return true;
}


bool WinterReflection::RenderScene(D3D* d3d, Camera* view, bool drawMirror)
{
	XMMATRIX viewMatrix, projectionMatrix;
	XMFLOAT3 cameraPos;
	bool result;

	// Get the view, and projection matrices from the camera.
	viewMatrix = view->GetViewMatrix();
	projectionMatrix = view->GetProjMatrix();

	cameraPos = view->GetPosition();

	m_passingTime += 1.f / 60.f;
	float radius = 5.f;
	float m_rotSpeed = 0.25f;

	XMMATRIX temp;

	Model* orbitSphere = m_resources->GetModel(ResourceManager::ORBIT_MODEL);
	temp = XMMatrixRotationRollPitchYaw((m_rotSpeed * m_passingTime), 0.f, 0.f);
	orbitSphere->SetWorldMatrix(XMMatrixTranslation(radius * cos(m_rotSpeed*m_passingTime), 0.f, radius * sin(m_rotSpeed * m_passingTime)));

	orbitSphere->SetWorldMatrix(XMMatrixMultiply(temp, orbitSphere->GetWorldMatrix()));
	temp = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	orbitSphere->SetWorldMatrix(XMMatrixMultiply(temp, orbitSphere->GetWorldMatrix()));

	//m_layers[FOREGROUND]->addChild(SceneNode::uSceneNode(orbitSphere));

	orbitSphere->Render(d3d->GetDeviceContext());

	result = m_shaders->RenderLight(orbitSphere, view, m_Light);
	if (!result)
	{
		return false;
	}


	//reflectiveSphere->SetEnabled(drawMirror);

	//m_layers[FOREGROUND]->addChild(SceneNode::uSceneNode(reflectiveSphere));

	if (drawMirror)
	{
		Model* reflectiveSphere = m_resources->GetModel(ResourceManager::REFLECTIVE_MODEL);
		reflectiveSphere->SetTexture(m_cMapSRV);
		reflectiveSphere->Render(d3d->GetDeviceContext());
		result = m_shaders->RenderReflection(reflectiveSphere, m_Camera);
		if (!result)
		{
			return false;
		}
		//This will try to use the texture but light.ps uses Texture2D not TextureCube
		/*result = m_shaders->RenderLight(reflectiveSphere, m_Camera, m_Light);
		if (!result)
		{
			return false;
		}*/
	}
	// Render the model using the light shader.

	Model* skySphere = m_resources->GetModel(ResourceManager::SKY_DOME_MODEL);

	skySphere->SetWorldMatrix(XMMatrixTranslation(cameraPos.x, cameraPos.y, cameraPos.z));

	//m_layers[SKY]->addChild(SceneNode::uSceneNode(skySphere));

	d3d->TurnOffCulling();
	d3d->SetDepthLessEqual();


	skySphere->Render(d3d->GetDeviceContext());
	result = m_shaders->RenderSkySphere(skySphere, view);
	if (!result)
	{
		return false;
	}

	d3d->TurnOnCulling();
	d3d->SetDepthLess();

	/*for (int i = 0; i < MAX_LAYER_COUNT; ++i)
	{
		if (i == SKY)
		{
			m_D3D->TurnOffCulling();
			m_D3D->SetDepthLessEqual();
			m_layers[i]->draw(m_D3D->GetDeviceContext());

			result = m_shaders->RenderSkySphere(skySphere, view);
			if (!result)
			{
				return false;
			}

			m_D3D->TurnOnCulling();
			m_D3D->SetDepthLess();
		}
		else
		{
			m_layers[i]->draw(m_D3D->GetDeviceContext());
		}
	}
*/
/*if (drawMirror)
{
	result = m_shaders->RenderReflection(reflectiveSphere, m_Camera);
	if(!result)
	{
		return false;
	}
}*/

//m_sceneGraph.draw(m_D3D->GetDeviceContext());

	return true;
}