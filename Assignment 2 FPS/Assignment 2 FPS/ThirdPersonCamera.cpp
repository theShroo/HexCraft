#include "ThirdPersonCamera.h"


using namespace DirectX;

ThirdPersonCamera::ThirdPersonCamera() : Camera() {
	m_objectToFollow = 0;
	m_offset = MathsHelper::ZeroVector3();
	m_targetOffset = MathsHelper::GetXMVECTOR3(0.0f, 10.0f, 0.0f);
}
ThirdPersonCamera::ThirdPersonCamera(GameObject* target, XMFLOAT3 offset) {

}


void ThirdPersonCamera::Update(float timestep) {
	if (m_objectToFollow) {
		XMVECTOR target = m_objectToFollow->GetPosition();
		XMVECTOR offset = XMVector4Transform(m_offset, XMMatrixRotationY(m_objectToFollow->GetYRotation()));
		SetLookAt(target + m_targetOffset);
		SetPosition(target + offset);
	}
	Camera::Update(timestep);
}


void ThirdPersonCamera::SetFollowTarget(GameObject* target, XMFLOAT3 offset) {
	m_objectToFollow = target;
	m_offset = XMLoadFloat3(&offset);
}


void ThirdPersonCamera::SetOffset(XMFLOAT3 offset) {
	m_offset = XMLoadFloat3(&offset);
}
void ThirdPersonCamera::SetOffset(XMVECTOR offset) {
	m_offset = offset;
}
