#include "PhysicsObject.h"
#include "Map.h"

using namespace DirectX;

void PhysicsObject::ApplyForce(XMVECTOR force) {
	m_acceleration += force;
}
void PhysicsObject::ApplyFriction(float strength) {
	ApplyForce((-m_velocity * strength));
}

PhysicsObject::PhysicsObject(std::string shaderID, std::string meshID, std::string textureID, XMVECTOR position) : GameObject(position, shaderID, meshID, textureID) {
	m_velocity = MathsHelper::ZeroVector3();
	m_acceleration = MathsHelper::ZeroVector3();
	m_frictionAmount = 0.1f;
	m_boundingBox.SetMin(m_position + (m_mesh[0]->GetMin() * m_scale));
	m_boundingBox.SetMax(m_position + (m_mesh[0]->GetMax() * m_scale));
	m_health = 1;
}



void PhysicsObject::Update(float timestep) {
	m_position += m_velocity;

	ApplyFriction(m_frictionAmount);
	m_velocity += m_acceleration;
	m_acceleration = MathsHelper::ZeroVector3();
	m_boundingBox.SetMin(m_position + m_mesh[0]->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh[0]->GetMax());

	GameObject::Update(timestep);
}

PhysicsObject::~PhysicsObject() {
}

void PhysicsObject::DoCollision(PhysicsObject* other, Map* map) {
	m_position = m_location->GetPosition();
	m_velocity = MathsHelper::ZeroVector3();

}

void PhysicsObject::SetMesh(std::string meshID, unsigned meshIndex) {
	GameObject::SetMesh(meshID, meshIndex);
		
		m_boundingBox.SetMin(m_position + m_mesh[0]->GetMin());
		m_boundingBox.SetMax(m_position + m_mesh[0]->GetMax());

}

void PhysicsObject::DoStanding(float surface, Map* map) {
	float offset = m_mesh[0]->GetBottom().m128_f32[1];
	if (m_position.m128_f32[1] < surface) {
		m_position.m128_f32[1] = surface - offset;
	}
	if (m_position.m128_f32[1] <= surface && m_velocity.m128_f32[1] < 0) {
		m_velocity.m128_f32[1] = 0;
	}

}