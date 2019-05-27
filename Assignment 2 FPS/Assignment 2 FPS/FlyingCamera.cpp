#include "FlyingCamera.h"

using namespace DirectX;



FlyingCamera::FlyingCamera(InputController* input, XMFLOAT3 startPos) :Camera() {

	m_moveSpeed = 5;
	m_rotationSpeed = 2;
	m_heightChangeSpeed = 3;
	m_heading = 0.0f;
	m_pitch = 0.0f;
	m_input = input;
	SetPosition(startPos);

}
void FlyingCamera::Update(float timestep) {
	m_heading += (m_input->GetMouseDeltaX() * m_rotationSpeed * timestep);
	m_pitch += (m_input->GetMouseDeltaY() * m_rotationSpeed * timestep);

	m_pitch = MathsHelper::Clamp(m_pitch, float(ToRadians(-80.0f)), float(ToRadians(80.0f)));

	XMMATRIX heading = XMMatrixRotationY(m_heading);
	XMMATRIX pitch = XMMatrixRotationX(m_pitch);
	XMVECTOR right = XMVector3TransformNormal(MathsHelper::RightVector3(), heading);
	XMVECTOR forward = XMVector3Cross(right, MathsHelper::UpVector3());
	XMVECTOR up = MathsHelper::UpVector3();

	if (m_input->GetKeyHold('W')) {

		Move(forward * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold('S')) {
		Move(-forward * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold('A')) {
		Move(-right * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold('D')) {
		Move(right * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold(VK_OEM_MINUS)) {
		Move(-up * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold(VK_OEM_PLUS)) {
		Move(up * m_moveSpeed * timestep);
	}
	XMMATRIX LookatRotation = pitch * heading;
	XMVECTOR lookat = XMVector3TransformNormal(MathsHelper::ForwardVector3() , LookatRotation);
	SetLookAt(lookat + GetPosition());

	Camera::Update(timestep);
}



