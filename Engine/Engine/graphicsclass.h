////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "Cubemap.h"
#include "inputclass.h"
#include "modelclass.h"
#include "LightShader.h"
#include "lightclass.h"
#include "OrbitSphere.h"
#include "ReflectShader.h"
#include "ReflectiveSphere.h"
#include "SkySphere.h"
#include "SkySphereShader.h"
#include "Text.h"

#include <DirectXMath.h>
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
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	void MoveCamera(D3DXVECTOR3);
	void RotateCamera(D3DXVECTOR3);

private:
	bool RenderWithoutSphere();
	bool Render(CameraClass*, bool);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	LightClass* m_Light;
	LightShader* m_LightShader;
	OrbitSphere* m_orbitSphere;
	ReflectShader* m_ReflectShader;
	ReflectiveSphere* m_reflectiveSphere;
	SkySphere* m_SkySphere;
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
	D3DXMATRIX m_view;
};

#endif