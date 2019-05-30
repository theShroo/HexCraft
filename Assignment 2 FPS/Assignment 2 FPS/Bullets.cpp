#include "Bullets.h"
#include "Collisions.h"
#include "Map.h"
#include "Cell.h"

// all bullets need access to the current map so that they can destroy themselves properly.
using namespace DirectX;


Map* Bullets::m_currentMap = 0;

void Bullets::Initialise(Map* map) {
	m_currentMap = map;
}


Bullets::Bullets(std::string shaderID, std::string meshID, std::string textureID, XMVECTOR position, XMVECTOR velocity, XMVECTOR rotation) : PhysicsObject(shaderID, meshID, textureID, position) {
	m_velocity = velocity;
	m_frictionAmount = 0;
	m_ttl = 100;
	m_leftGun = false;
	m_rotation = rotation;
	if (m_currentMap) {
		m_currentMap->AddObject(this);
	}
	else {
		RaiseException(7, 0, 0, 0);
	}
}


Bullets::~Bullets() {

}


void Bullets::DoCollision(PhysicsObject* other, Map* Map) {
	Map->RemoveObject(this);
	other->Damage(5);
}

void Bullets::DoCollision(Cell* other, Map* Map) {
	Map->RemoveObject(this);
	other->Break(5, 0);
}

void Bullets::Update(float timeStep) {
	m_ttl -= timeStep;
	m_position += m_velocity;

	ApplyFriction(m_frictionAmount);
	m_velocity += m_acceleration;
	m_acceleration = MathsHelper::ZeroVector3();
	m_boundingBox.SetMin(m_position + m_mesh[0]->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh[0]->GetMax());

	GameObject::Update(timeStep);
}


Impact::Impact(XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID) : GameObject(position, shaderID, meshID, textureID) {
	m_ttl = 2;
	if (Bullets::m_currentMap) {
		Bullets::m_currentMap->AddObject(this);
	}
	else {
		RaiseException(7, 0, 0, 0);
	}

}

Impact::~Impact() {}


void Impact::Update(float timestep) {
	Resize(10.0f* timestep);
	GameObject::Update(timestep);

	if (m_ttl > 0) {
		m_ttl -= timestep;
	}
	else {
		Bullets::m_currentMap->RemoveObject(this);
		Impact::~Impact();
	}
}
