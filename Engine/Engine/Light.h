////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class Light
{
public:
	Light();
	Light(const Light&);
	~Light();

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetAmbientColour(float, float, float, float);
	void SetSpecColour(float, float, float, float);
	void SetSpecIntensity(float);

	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetAmbientColour();
	XMFLOAT3 GetDirection();
	XMFLOAT4 GetSpecColour();
	float GetSpecIntensity();

private:
	XMFLOAT4 m_ambientColour;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_specColour;
	float m_specIntensity;
};

#endif