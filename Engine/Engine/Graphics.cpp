////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics() : m_rotSpeed(0.25f), m_passingTime(0.f), m_rotation(0.f), m_fps(0), m_startTime(0), m_count(0), m_sceneGraph()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Light = 0;
	m_text = 0;
	m_cMap = 0;
	m_resources = 0;
	m_shaders = 0;
	m_startTime = timeGetTime();
}

Graphics::Graphics(const Graphics& other)
{
}


Graphics::~Graphics()
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

	if (m_cMap)
	{
		m_cMap->Shutdown();
		delete m_cMap;
		m_cMap = 0;
	}

	if (m_text)
	{
		m_text->Shutdown();
		delete m_text;
		m_text = 0;
	}

	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		delete m_D3D;
		m_D3D = 0;
	}
}


bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_hwnd = hwnd;

	bool result;
	XMMATRIX viewMatrix;

	// Create the Direct3D object.
	m_D3D = new D3D;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new Camera;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	viewMatrix = m_Camera->GetViewMatrix();
	m_view = viewMatrix;

	m_text = new Text;
	if (!m_text)
		return false;


	m_resources = new ResourceManager(m_D3D->GetDevice(), m_D3D->GetDeviceContext());
	if (!m_resources->LoadResources())
	{
		return false;
	}

	m_shaders = new ShaderManager(m_D3D->GetDevice(), m_D3D->GetDeviceContext());
	if (!m_shaders->InitialiseShaders())
		return false;

	result = m_text->Initialise(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, viewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the text object", L"Error", MB_OK);
		return false;
	}
	// Create the light object.
	m_Light = new Light;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetDiffuseColor(.9f, .9f, .9f, 1.f);
	m_Light->SetAmbientColour(0.85f, .85f, .85f, 1.f);
	m_Light->SetDirection(.75f, .0f, 1.0f);
	m_Light->SetSpecIntensity(20.0f);
	m_Light->SetSpecColour(1.0f, 1.0f, 1.0f, 1.0f);

	m_cMap = new Cubemap;
	if (!m_cMap)
		return false;

	for (int i = 0; i < MAX_LAYER_COUNT; ++i)
	{
		SceneNode::uSceneNode layer(new SceneNode());
		m_layers[i] = layer.get();
		m_sceneGraph.addChild(std::move(layer));
	}


	return true;
}


bool Graphics::Frame()
{
	++m_count;
	bool result;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;
		m_startTime = timeGetTime();
	}
	// Render the graphics scene.
	result = RenderCubemap();
	if(!result)
	{
		return false;
	}

	return true;
}


bool Graphics::RenderCubemap()
{	
	D3D11_VIEWPORT vport;
	UINT num_viewps = 1;
	ID3D11RenderTargetView* defRTV = m_D3D->getRenderTargetView();
	ID3D11DepthStencilView* defDSV = m_D3D->getDepthStencilView();
	ID3D11RenderTargetView* cMapRTV = 0;

	m_D3D->GetDeviceContext()->RSGetViewports(&(num_viewps), &vport);
	m_cMap->Initialise(m_D3D->GetDevice());

	m_D3D->BeginScene(0.0f, 0.0f, 1.0f, 1.0f);

	m_D3D->GetDeviceContext()->RSSetViewports(1, &(m_cMap->getViewport()));
	m_cMap->InitCameras(0.f, 0.f, 0.f);
	float colour[4] { 1.f, 0.0f, 0.0f, 1.f };

	for (int i = 0; i < 6; ++i)
	{
		cMapRTV = m_cMap->getRTV(i);

		m_D3D->GetDeviceContext()->ClearRenderTargetView(cMapRTV, colour);
		m_D3D->GetDeviceContext()->ClearDepthStencilView(m_cMap->getDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &(cMapRTV), m_cMap->getDSV());
		Render(m_cMap->getView(i), false);
	}

	m_D3D->GetDeviceContext()->RSSetViewports(1, &vport);
	m_D3D->GetDeviceContext()->OMSetRenderTargets(1, &(defRTV), defDSV);
	m_D3D->GetDeviceContext()->GenerateMips(m_cMap->getSRV());

	m_D3D->GetDeviceContext()->ClearRenderTargetView(defRTV, colour);
	m_D3D->GetDeviceContext()->ClearDepthStencilView(defDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_cMapSRV = m_cMap->getSRV();
	XMMATRIX projMatrix;
	m_D3D->GetProjectionMatrix(projMatrix);
	m_Camera->setProjMatrix(projMatrix);
	m_Camera->Render();
	Render(m_Camera, true); 

	XMMATRIX worldMatrix, orthoMatrix;
	m_D3D->GetOrthoMatrix(orthoMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	m_D3D->TurnZBufferOff();
	m_D3D->TurnOnAlphaBlending();

	bool res;
	std::string fpsSentence = "FPS: ";
	fpsSentence += std::to_string(m_fps);
	res = m_text->setText(fpsSentence.c_str(), m_D3D->GetDeviceContext());
	res = m_text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!res)
		return false;

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();
	// Present the rendered scene to the screen.
	
	m_D3D->EndScene();
	return true;
}


bool Graphics::Render(Camera* view, bool drawMirror)
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

	XMMATRIX temp;

	Model* orbitSphere = m_resources->GetModel(ResourceManager::ORBIT_MODEL);
	temp = XMMatrixRotationRollPitchYaw((m_rotSpeed * m_passingTime), 0.f, 0.f);
	orbitSphere->SetWorldMatrix(XMMatrixTranslation(radius * cos(m_rotSpeed*m_passingTime), 0.f, radius * sin(m_rotSpeed * m_passingTime)));

	orbitSphere->SetWorldMatrix(XMMatrixMultiply(temp, orbitSphere->GetWorldMatrix()));
	temp = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	orbitSphere->SetWorldMatrix(XMMatrixMultiply(temp, orbitSphere->GetWorldMatrix()));

	m_layers[FOREGROUND]->addChild(SceneNode::uSceneNode(orbitSphere));

	//orbitSphere->Render(m_D3D->GetDeviceContext());
	result = m_shaders->RenderLight(orbitSphere, view, m_Light);
	if (!result)
	{
		return false;
	}

	Model* reflectiveSphere = m_resources->GetModel(ResourceManager::REFLECTIVE_MODEL);

	reflectiveSphere->SetEnabled(drawMirror);
	reflectiveSphere->SetTexture(m_cMapSRV);

	m_layers[FOREGROUND]->addChild(SceneNode::uSceneNode(reflectiveSphere));

	// Render the model using the light shader.
	//reflectiveSphere->Render(m_D3D->GetDeviceContext());
	result = m_shaders->RenderReflection(reflectiveSphere, m_Camera);
	if(!result)
	{
		return false;
	}
	/*result = m_shaders->RenderLight(reflectiveSphere, m_Camera, m_Light);
	if (!result)
	{
		return false;
	}*/

	Model* skySphere = m_resources->GetModel(ResourceManager::SKY_DOME_MODEL);

	skySphere->SetWorldMatrix(XMMatrixTranslation(cameraPos.x, cameraPos.y, cameraPos.z));

	m_layers[SKY]->addChild(SceneNode::uSceneNode(skySphere));

	m_D3D->TurnOffCulling();
	m_D3D->SetDepthLessEqual();


	//skySphere->Render(m_D3D->GetDeviceContext());
	result = m_shaders->RenderSkySphere(skySphere, view);
	if (!result)
	{
		return false;
	}

	m_D3D->TurnOnCulling();
	m_D3D->SetDepthLess();

	m_sceneGraph.draw(m_D3D->GetDeviceContext());

	return true;
}

void Graphics::MoveCamera(XMFLOAT3 movement)
{
	m_Camera->Move(movement);
}

void Graphics::RotateCamera(XMFLOAT3 rotation)
{
	m_Camera->Rotate(rotation);
}