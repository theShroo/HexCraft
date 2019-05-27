/*	FIT2096 - Example Code
*	Camera.cpp
*	Created by Elliott Wilson - 2015 - Monash University
*	Implementation of Camera.h
*/

#include "Camera.h"

using namespace DirectX;
Camera::Camera()
{
	//Set up a "default" camera!
	// now using directX prefered vectors. requires a little extra work here.
	m_position = MathsHelper::GetXMVECTOR3(0.0f, 10.0f, -30.0f);
	m_lookAtTarget = MathsHelper::ZeroVector3();
	m_up = MathsHelper::UpVector3();

	m_aspectRatio = 1280.0f / 720.0f;
	m_fieldOfView = (float)ToRadians(45.0f);
	m_nearClip = 0.1f;
	m_farClip = 1000.0f;

	m_viewDirty = true;
	m_projectionDirty = true;
}

Camera::Camera(XMVECTOR pos, XMVECTOR lookAt, XMVECTOR up, float aspect, float fov, float nearClip, float farClip)
{
	m_position = pos;
	m_lookAtTarget = lookAt;
	m_up = up;

	m_aspectRatio = aspect;
	m_fieldOfView = fov;
	m_nearClip = nearClip;
	m_farClip = farClip;

	m_viewDirty = true;
	m_projectionDirty = true;
}

Camera::~Camera()
{

}

void Camera::SetPosition(XMFLOAT3 pos)
{
	m_position = XMLoadFloat3(&pos);
	m_viewDirty = true;		//Every time a value is changed then the respective matrix is set "dirty"
}

// overloads for XMVECTOR types. much faster than XMFloats
void Camera::SetPosition(XMVECTOR pos)
{
	m_position = pos;
	m_viewDirty = true;		
}


void Camera::SetLookAt(XMFLOAT3 lookAt)
{
	m_lookAtTarget = XMLoadFloat3(&lookAt);
	m_viewDirty = true;
}
void Camera::SetLookAt(XMVECTOR lookAt)
{
	m_lookAtTarget = lookAt;
	m_viewDirty = true;
}
void Camera::SetUp(XMFLOAT3 up)
{
	m_up = XMLoadFloat3(&up);
	m_viewDirty = true;
}

void Camera::SetAspectRatio(float aspect)
{
	m_aspectRatio = aspect;
	m_projectionDirty = true;
}

void Camera::SetFieldOfView(float fov)
{
	m_fieldOfView = fov;
	m_projectionDirty = true;
}

void Camera::SetNearClip(float nearClip)
{
	m_nearClip = nearClip;
	m_projectionDirty = true;
}

void Camera::SetFarClip(float farClip)
{
	m_farClip = farClip;
	m_projectionDirty = true;
}

void Camera::Update(float timestep)
{

	if (m_viewDirty)	//We will only recalculate a matrix if it is "dirty"
	{


		if (!XMVector3Equal(m_position, m_lookAtTarget)) {
			m_view = XMMatrixLookAtLH(m_position, m_lookAtTarget, m_up);
			m_viewDirty = false;	//Once we recalculate the matrix then it is no longer dirty!
		}
	}
	if (m_projectionDirty)
	{
		m_projection = DirectX::XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearClip, m_farClip);
		m_projectionDirty = false;
	}
}

XMVECTOR Camera::GetPosition() {	
	return m_position;
}

void Camera::Move(XMVECTOR displacement) {
	m_position += displacement;
}
