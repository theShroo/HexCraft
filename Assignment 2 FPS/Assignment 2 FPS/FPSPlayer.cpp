#include "FPSPlayer.h"

using namespace DirectX;


FPSPlayer::FPSPlayer(XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID, InputController* input) : Player(position, shaderID, meshID, textureID, input) {
	m_moveSpeed = 2.5f;
	m_rotationSpeed = 1;
	m_heading = 0.0f;
	m_pitch = 0.0f;
	m_height = 1.8f;
	m_frictionAmount = 0.55f;
	m_input = input;
	m_position = position;
	m_camera = new Camera();
}

void FPSPlayer::Update(float timestep) {
	m_heading += (m_input->GetMouseDeltaX() * m_rotationSpeed * timestep);
	if (m_heading > PI*2) {
		m_heading -= PI * 2.0f;
	}
	if (m_heading < -PI * 2) {
		m_heading += PI * 2.0f;
	}
	m_rotation = MathsHelper::GetXMVECTOR3(0, m_heading, 0);
	m_pitch += (m_input->GetMouseDeltaY() * m_rotationSpeed *timestep);

	m_pitch = MathsHelper::Clamp(m_pitch, ToRadians(-80.0f), ToRadians(80.0f));

	XMMATRIX heading = XMMatrixRotationY(m_heading);
	XMMATRIX pitch = XMMatrixRotationX(m_pitch);
	XMVECTOR right = XMVector3TransformNormal(MathsHelper::RightVector3(), heading);
	XMVECTOR forward = XMVector3Cross(right, MathsHelper::UpVector3());
	XMVECTOR up = MathsHelper::UpVector3();

	XMMATRIX LookatRotation = pitch * heading;
	XMVECTOR lookat = XMVector3TransformNormal(MathsHelper::ForwardVector3(), LookatRotation);
	XMVECTOR height = MathsHelper::GetXMVECTOR3(0, m_height, 0);

	if (m_input->GetKeyHold('W')) {

		ApplyForce(forward * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold('S')) {
		ApplyForce(-forward * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold('A')) {
		ApplyForce(-right * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold('D')) {
		ApplyForce(right * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold(VK_OEM_MINUS)) {
		ApplyForce(-up * m_moveSpeed * timestep);
	}
	if (m_input->GetKeyHold(VK_OEM_PLUS)) {
		ApplyForce(up * m_moveSpeed * timestep);
	}
	if (m_input->GetMouseDown(LEFT_MOUSE)) {
		DoAttack(lookat);
	}
	if (m_input->GetKeyDown(VK_SPACE)) {
		ApplyForce(MathsHelper::GetXMVECTOR3(0, 1.0f, 0));
	}
	// update the player (and its corresponding game objects) before repositioning the camera.
	// the position difference would be minimal, but lets quash these errors before they occur.
	Player::Update(timestep);
	m_camera->SetPosition(m_position + height);
	m_camera->SetLookAt(lookat + m_camera->GetPosition());
	
	m_camera->Update(timestep);

}

FPSPlayer::~FPSPlayer() {
	delete m_camera;
}

Camera* FPSPlayer::GetCamera() {
	return m_camera;
}

void FPSPlayer::Render(Direct3D* renderer, Camera* cam) {
	// the FPSPlayer doesnt render its meshes, so this version of the virtual call render is empty.
 }
