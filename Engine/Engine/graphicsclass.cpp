////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass() : m_rotSpeed(0.25f), m_passingTime(0.f), m_rotation(0.f), m_fps(0), m_startTime(0), m_count(0)
{
	m_D3D = 0;
	m_Camera = 0;
	m_reflectiveSphere = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_SkySphere = 0;
	m_SkySphereShader = 0;
	m_text = 0;
	m_cMap = 0;

	m_startTime = timeGetTime();
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_hwnd = hwnd;

	bool result;
	D3DXMATRIX viewMatrix;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
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
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(viewMatrix);
	m_view = viewMatrix;

	m_text = new Text;
	if (!m_text)
		return false;

	result = m_text->Initialise(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, viewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the text object", L"Error", MB_OK);
		return false;
	}

	m_orbitSphere = new OrbitSphere("../Engine/data/sphere.txt");
	if (!m_orbitSphere)
		return false;

	result = m_orbitSphere->Initialize(m_D3D->GetDevice(), L"../Engine/data/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	// Create the light shader object.
	m_LightShader = new LightShader;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the reflective sphere object.
	m_reflectiveSphere = new ReflectiveSphere("../Engine/data/sphere.txt");
	if(!m_reflectiveSphere)
	{
		return false;
	}

	// Initialize the model object.
	result = m_reflectiveSphere->Initialize(m_D3D->GetDevice(), L"../Engine/data/seafloor.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_ReflectShader = new ReflectShader;
	if (!m_ReflectShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_ReflectShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the light object.
	m_Light = new LightClass;
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

	m_SkySphere = new SkySphere("../Engine/data/skydome.txt");
	if (!m_SkySphere)
		return false;

	//Left over from initial sky sphere creation
	/*m_SkySphere->setApexColour(0.0f, 0.05f, 0.6f, 1.0f);
	m_SkySphere->setCentreColour(0.0f, 0.5f, 0.8f, 1.0f);*/

	result = m_SkySphere->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the sky sphere object", L"Error", MB_OK);
		return false;
	}

	m_SkySphereShader = new SkySphereShader;
	if (!m_SkySphereShader)
		return false;

	result = m_SkySphereShader->Initialise(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the sky shader object", L"Error", MB_OK);
		return false;
	}

	m_cMap = new Cubemap;
	if (!m_cMap)
		return false;


	return true;
}


void GraphicsClass::Shutdown()
{

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

	if (m_SkySphere)
	{
		m_SkySphereShader->Shutdown();
		delete m_SkySphereShader;
		m_SkySphereShader = 0;
	}

	if (m_SkySphere)
	{
		m_SkySphere->Shutdown();
		delete m_SkySphere;
		m_SkySphere = 0;
	}

	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the reflective sphere object.
	if(m_reflectiveSphere)
	{
		m_reflectiveSphere->Shutdown();
		delete m_reflectiveSphere;
		m_reflectiveSphere = 0;
	}

	//Release the orbiting sphere object
	if (m_orbitSphere)
	{
		m_orbitSphere->Shutdown();
		delete m_orbitSphere;
		m_orbitSphere = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
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
	result = RenderWithoutSphere();
	if(!result)
	{
		return false;
	}

	return true;
}




//render without sphere should be called called where "draw" is it shouldn't include the set up code
//it gets clled when everything is set up and then we draw the whole scene after completion
bool GraphicsClass::RenderWithoutSphere()
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
	D3DXMATRIX projMatrix;
	m_D3D->GetProjectionMatrix(projMatrix);
	m_Camera->setProjMatrix(projMatrix);
	m_Camera->Render();
	Render(m_Camera, true); 

	D3DXMATRIX worldMatrix, orthoMatrix;
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


bool GraphicsClass::Render(CameraClass* view, bool drawMirror)
{
	D3DXMATRIX viewMatrix, projectionMatrix;
	D3DXVECTOR3 cameraPos;
	bool result;	
	
	

	// Get the view, and projection matrices from the camera.
	view->GetViewMatrix(viewMatrix);	
	projectionMatrix = view->GetProjMatrix();

	cameraPos = view->GetPosition();
	
	m_passingTime += 1.f / 60.f;
	float radius = 5.f;

	D3DXMATRIX temp;

	D3DXMatrixRotationYawPitchRoll(&(temp), (m_rotSpeed * m_passingTime), 0.f, 0.f);
	D3DXMatrixTranslation(&(m_orbitSphere->getWorldMatrix()), radius * cos(m_rotSpeed*m_passingTime), 0.f , radius * sin(m_rotSpeed * m_passingTime));

	D3DXMatrixMultiply(&m_orbitSphere->getWorldMatrix(), &temp, &m_orbitSphere->getWorldMatrix());
	D3DXMatrixScaling(&temp, 0.5f, 0.5f, 0.5f);
	D3DXMatrixMultiply(&m_orbitSphere->getWorldMatrix(), &temp, &m_orbitSphere->getWorldMatrix());


	m_orbitSphere->Render(m_D3D->GetDeviceContext());
	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_orbitSphere->GetTexture(), m_orbitSphere->GetIndexCount(), m_orbitSphere->getWorldMatrix(), viewMatrix, projectionMatrix,
		m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColour(), m_Camera->GetPosition(),
		m_Light->GetSpecColour(), m_Light->GetSpecIntensity());
	if (!result)
	{
		return false;
	}


	if (drawMirror)
	{
		// Render the model using the light shader.
		m_reflectiveSphere->Render(m_D3D->GetDeviceContext());
		result = m_ReflectShader->Render(m_D3D->GetDeviceContext(), m_reflectiveSphere->GetTexture(), m_reflectiveSphere->GetIndexCount(), m_reflectiveSphere->getWorldMatrix(), viewMatrix, projectionMatrix, 
										m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColour(), m_Camera->GetPosition(), 
										m_Light->GetSpecColour(), m_Light->GetSpecIntensity(), m_cMapSRV);
		if(!result)
		{
			return false;
		}
	}

	D3DXMatrixTranslation(&(m_SkySphere->getWorldMatrix()), cameraPos.x, cameraPos.y, cameraPos.z);

	m_D3D->TurnOffCulling();
	m_D3D->SetDepthLessEqual();


	m_SkySphere->Render(m_D3D->GetDeviceContext());
	result = m_SkySphereShader->Render(m_D3D->GetDeviceContext(), m_SkySphere->GetIndexCount(), m_SkySphere->getWorldMatrix(), viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	m_D3D->TurnOnCulling();
	m_D3D->SetDepthLess();

	return true;
}

void GraphicsClass::MoveCamera(D3DXVECTOR3 movement)
{
	m_Camera->Move(movement);
}

void GraphicsClass::RotateCamera(D3DXVECTOR3 rotation)
{
	m_Camera->Rotate(rotation);
}