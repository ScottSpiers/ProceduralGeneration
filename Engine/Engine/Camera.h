////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <directxmath.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	enum class CamType
	{
		FREE,
		FPC,
		TPC,
		MAX_CAM_TYPES
	};

public:
	Camera();
	Camera(CamType);
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	XMFLOAT3 GetNextPos(XMFLOAT3);

	void Render();
	void setViewMatrix(XMMATRIX);
	XMMATRIX GetViewMatrix();
	void setProjMatrix(XMMATRIX);
	XMMATRIX GetProjMatrix() const;

	void Move(XMFLOAT3);
	void Rotate(XMFLOAT3);

	void SetCamType(CamType);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix , m_projMatrix;

	CamType m_type;
};

#endif