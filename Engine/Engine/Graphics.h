////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "D3D.h"
#include "Input.h"
#include "Model.h"
#include "Scene.h"
#include "Text.h"
#include "WinterReflection.h"

#include <directxmath.h>
#include <string>

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
	bool Render();

private:
	D3D* m_D3D;
	Scene* m_Scene;
	LightShader* m_LightShader;
	ReflectShader* m_ReflectShader;
	SkySphereShader* m_SkySphereShader;
	Text* m_text;

	int m_fps, m_count;
	unsigned long m_startTime;

	HWND m_hwnd;
	int m_screenWidth, m_screenHeight;
	XMMATRIX m_view;
};

#endif