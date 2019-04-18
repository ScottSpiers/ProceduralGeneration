////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Camera.h"


Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
	
}


Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}


void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Rotate(XMFLOAT3 rotation)
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

void Camera::Move(XMFLOAT3 movement)
{
	XMMATRIX rotMatrix;
	XMVECTOR right{ 1.0f, 0.0f, 0.0f };
	XMVECTOR up{ 0.0f, 1.0f, 0.0f };
	XMVECTOR forward{ 0.0f, 0.0f, 1.0f };

	float pitch = m_rotationX * 0.0174532925f;
	float yaw = m_rotationY * 0.0174532925f;
	float roll = m_rotationZ * 0.0174532925f;
	
	float y = m_positionY;

	rotMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	XMVECTOR target;
	target = XMVector3TransformCoord(forward, rotMatrix);
	target = XMVector3Normalize(target);

	right = XMVector3TransformCoord(right, rotMatrix);
	forward = XMVector3TransformCoord(forward, rotMatrix);
	up = XMVector3Cross(forward, right);

	XMVECTOR pos{ m_positionX, m_positionY, m_positionZ };
	
	pos = XMVectorAdd(pos, movement.x * right);
	pos = XMVectorAdd(pos, 0.0f * up);
	pos = XMVectorAdd(pos, movement.z * forward);

	target = XMVectorAdd(pos, target);

	m_viewMatrix = XMMatrixLookAtLH(pos, target, up);
	m_positionX = XMVectorGetByIndex(pos, 0);
	//m_positionY = XMVectorGetByIndex(pos, 1);
	m_positionZ = XMVectorGetByIndex(pos, 2);
	
}


void Camera::Render()
{
	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	// Setup the position of the camera in the world.
	position = XMVectorSet(m_positionX, m_positionY, m_positionZ, 1.0f);

	// Setup where the camera is looking by default.
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = XMVectorAdd(position, lookAt);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	
	return;
}


void Camera::setViewMatrix(XMMATRIX viewMatrix)
{
	m_viewMatrix = viewMatrix;
}

XMMATRIX Camera::GetViewMatrix()
{
	return m_viewMatrix;
}

void Camera::setProjMatrix(XMMATRIX projMatrix)
{
	m_projMatrix = projMatrix;
}

XMMATRIX Camera::GetProjMatrix() const
{
	return m_projMatrix;
}