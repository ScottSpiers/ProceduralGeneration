////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "D3D.h"
#include "Camera.h"
#include "Cubemap.h"
#include "Input.h"
#include "Model.h"
#include "LightShader.h"
#include "Light.h"
#include "ReflectShader.h"
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "SkySphereShader.h"
#include "Text.h"

#include <directxmath.h>
#include <string>

//TEMP
#include "SceneNode.h"
#include <array>

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(int, int, HWND);
	bool Frame();

	void MoveCamera(XMFLOAT3);
	void RotateCamera(XMFLOAT3);

private:
	bool RenderCubemap();
	bool Render(Camera*, bool);

private:
	D3D* m_D3D;
	Camera* m_Camera;
	Light* m_Light;
	LightShader* m_LightShader;
	ReflectShader* m_ReflectShader;
	SkySphereShader* m_SkySphereShader;
	Text* m_text;

	Cubemap* m_cMap;
	ID3D11ShaderResourceView* m_cMapSRV;

	float m_passingTime;
	float m_rotSpeed;
	float m_rotation;

	int m_fps, m_count;
	unsigned long m_startTime;

	HWND m_hwnd;
	int m_screenWidth, m_screenHeight;
	XMMATRIX m_view;

	ResourceManager* m_resources;
	ShaderManager* m_shaders;

	//TEMP
	enum Layers
	{
		BACKGROUND,
		FOREGROUND,
		SKY,
		MAX_LAYER_COUNT
	};


	SceneNode m_sceneGraph;
	std::array<SceneNode*, MAX_LAYER_COUNT> m_layers;
};

#endif