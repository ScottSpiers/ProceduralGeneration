////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}

void LightClass::SetAmbientColour(float red, float green, float blue, float alpha)
{
	m_ambientColour = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecColour(float red, float green, float blue, float alpha)
{
	m_specColour = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecIntensity(float power)
{
	m_specIntensity = power;
	return;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

D3DXVECTOR4 LightClass::GetAmbientColour()
{
	return m_ambientColour;
}

D3DXVECTOR3 LightClass::GetDirection()
{
	return m_direction;
}

D3DXVECTOR4 LightClass::GetSpecColour()
{
	return m_specColour;
}

float LightClass::GetSpecIntensity()
{
	return m_specIntensity;
}