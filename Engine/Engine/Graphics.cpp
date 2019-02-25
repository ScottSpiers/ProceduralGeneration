////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Graphics.h"


Graphics::Graphics() : m_fps(0), m_startTime(0), m_count(0)
{
	m_D3D = 0;
	m_text = 0;
	m_startTime = timeGetTime();
}

Graphics::Graphics(const Graphics& other)
{
}


Graphics::~Graphics()
{
	if (m_text)
	{
		m_text->Shutdown();
		delete m_text;
		m_text = 0;
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

	m_Scene = new WinterReflection(ResourceManager::WINTER_REFLECTION);
	result = m_Scene->Initialise(m_D3D->GetDevice(), m_D3D->GetDeviceContext());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the Scene", L"Error", MB_OK);
		return false;
	}
	viewMatrix = m_Scene->GetView();

	m_text = new Text();
	result = m_text->Initialise(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, viewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialise the text object", L"Error", MB_OK);
		return false;
	}

	/*for (int i = 0; i < MAX_LAYER_COUNT; ++i)
	{
		SceneNode::uSceneNode layer(new SceneNode());
		m_layers[i] = layer.get();
		m_sceneGraph.addChild(std::move(layer));
	}*/


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
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}


bool Graphics::Render()
{	
	bool res;

	res = m_Scene->Render(m_D3D);
	if (!res)
		return false;

	m_D3D->TurnZBufferOff();
	m_D3D->TurnOnAlphaBlending();

	XMMATRIX worldMatrix, orthoMatrix;
	m_D3D->GetOrthoMatrix(orthoMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	std::string fpsSentence = "FPS: ";
	fpsSentence += std::to_string(m_fps);
	res = m_text->setText(fpsSentence.c_str(), m_D3D->GetDeviceContext());
	res = m_text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!res)
		return false;

	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();

	m_D3D->EndScene();
	return true;
}

void Graphics::MoveCamera(XMFLOAT3 movement)
{
	m_Scene->MoveCamera(movement);
}

void Graphics::RotateCamera(XMFLOAT3 rotation)
{
	m_Scene->RotateCamera(rotation);
}