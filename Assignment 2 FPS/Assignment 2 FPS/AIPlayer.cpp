#include "AIPlayer.h"
#include "AIController.h"
#include "Bullets.h"

using namespace DirectX;

AIPlayer::AIPlayer(XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID, int type, float range) : PhysicsObject(shaderID, meshID, textureID, position) {
	m_AI = new AIController();;
	m_currentTarget = 0;
	m_target = m_AI->GetNextTarget(this);
	m_ROF = 1.0f/2.0f;
	m_cooldown = 5;
	m_heading = 0;
	m_health = 50;
	m_type = type;
	m_AI->SetFollowDistance(range);
}

AIPlayer::~AIPlayer() {
	if (m_AI) { delete m_AI; }
}


void AIPlayer::Update(float timestep) {
	float length;
	XMVECTOR direction = (m_target - m_position);
	XMStoreFloat(&length, XMVector3Length(direction));
	direction = XMVector3Normalize(direction);
	XMFLOAT3 facing;
	XMStoreFloat3(&facing, m_currentTarget->GetPosition() - m_position);
	m_heading = atan2(facing.x, facing.z);

	m_rotation = MathsHelper::GetXMVECTOR3(0.0f, m_heading, 0.0f);
	if (length > timestep * m_movespeed) {
		m_position = m_position + (direction * (timestep * m_movespeed));
	}
	else {
		m_position = m_target;
		m_target = m_AI->GetNextTarget(this);
	}
	// do shooting.
	if (m_cooldown > 0) {
		m_cooldown -= timestep;
	}
	else {
		// no reference to this object is kept here, bullets add tehmselves to the map array, and are deleted from it by the map object.
		new Bullets("Unlit Texture Shader", 
			"Bullet", 
			"Bullet", 
			XMVector3Transform(MathsHelper::GetXMVECTOR3(-0.133f, 1.2f, 0.137f), XMMatrixRotationRollPitchYawFromVector(m_rotation)) + m_position,
			XMVector3Normalize( m_currentTarget->GetPosition() - m_position)* 3.0f,
			m_rotation);

		m_cooldown = 1.0f / m_ROF;
	}
	PhysicsObject::Update(timestep);
}


void AIPlayer::SetTarget(GameObject* target) {
	m_currentTarget = target;
}


void AIPlayer::DoCollision(PhysicsObject* other, Map* map) {
	PhysicsObject::DoCollision(other, map);
	m_target = m_AI->GetNextTarget(this);
}


void AIPlayer::AddPatrolPoint(XMVECTOR location) {
	m_AI->AddPatrolPoint(location);

}