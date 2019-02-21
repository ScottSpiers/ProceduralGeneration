////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Light.h"


Light::Light()
{
}


Light::Light(const Light& other)
{
}


Light::~Light()
{
}


void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void Light::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}

void Light::SetAmbientColour(float red, float green, float blue, float alpha)
{
	m_ambientColour = XMFLOAT4(red, green, blue, alpha);
	return;
}

void Light::SetSpecColour(float red, float green, float blue, float alpha)
{
	m_specColour = XMFLOAT4(red, green, blue, alpha);
	return;
}

void Light::SetSpecIntensity(float power)
{
	m_specIntensity = power;
	return;
}

XMFLOAT4 Light::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT4 Light::GetAmbientColour()
{
	return m_ambientColour;
}

XMFLOAT3 Light::GetDirection()
{
	return m_direction;
}

XMFLOAT4 Light::GetSpecColour()
{
	return m_specColour;
}

float Light::GetSpecIntensity()
{
	return m_specIntensity;
}