////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}


D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Rotate(D3DXVECTOR3 rotation)
{
	m_rotationX += rotation.y;
	m_rotationY += rotation.x;
	m_rotationZ += rotation.z;

	if (m_rotationX < 0.f)
		m_rotationX = +360.f;
	else if (m_rotationX > 360.f)
		m_rotationX -= 360.f;

	if (m_rotationY < 0.0f)
		m_rotationY += 360.f;
	else if (m_rotationY > 360.f)
		m_rotationY -= 360.f;
}

void CameraClass::Move(D3DXVECTOR3 movement)
{
	D3DXMATRIX rotMatrix;
	D3DXVECTOR3 right{ 1.0f, 0.0f, 0.0f };
	D3DXVECTOR3 up{ 0.0f, 1.0f, 0.0f };
	D3DXVECTOR3 forward{ 0.0f, 0.0f, 1.0f };

	float pitch = m_rotationX * 0.0174532925f;
	float yaw = m_rotationY * 0.0174532925f;
	float roll = m_rotationZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotMatrix, yaw, pitch, roll);

	D3DXVECTOR3 target;
	D3DXVec3TransformCoord(&target, &forward, &rotMatrix);
	D3DXVec3Normalize(&target, &target);

	D3DXVec3TransformCoord(&right, &right, &rotMatrix);
	D3DXVec3TransformCoord(&forward, &forward, &rotMatrix);
	D3DXVec3Cross(&up, &forward, &right);

	D3DXVECTOR3 pos{ m_positionX, m_positionY, m_positionZ };
	
	D3DXVec3Add(&pos, &pos, &(movement.x * right));
	D3DXVec3Add(&pos, &pos, &(movement.y* up));
	D3DXVec3Add(&pos, &pos, &(movement.z * forward));

	target = pos + target;

	D3DXMatrixLookAtLH(&m_viewMatrix, &pos, &target, &up);
	m_positionX = pos.x;
	m_positionY = pos.y;
	m_positionZ = pos.z;
	
}


void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	
	return;
}


void CameraClass::setViewMatrix(D3DXMATRIX& viewMatrix)
{
	m_viewMatrix = viewMatrix;
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void CameraClass::setProjMatrix(D3DXMATRIX& projMatrix)
{
	m_projMatrix = projMatrix;
}

D3DXMATRIX CameraClass::GetProjMatrix() const
{
	return m_projMatrix;
}