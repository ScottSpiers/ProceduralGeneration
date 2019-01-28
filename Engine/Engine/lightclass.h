////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetAmbientColour(float, float, float, float);
	void SetSpecColour(float, float, float, float);
	void SetSpecIntensity(float);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetAmbientColour();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecColour();
	float GetSpecIntensity();

private:
	D3DXVECTOR4 m_ambientColour;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_specColour;
	float m_specIntensity;
};

#endif