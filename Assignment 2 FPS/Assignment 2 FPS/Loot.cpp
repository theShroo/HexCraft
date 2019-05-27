#include "Loot.h"
#include "Map.h"
#include "FPSPlayer.h"
using namespace DirectX;

Loot::Loot(std::string shaderID, std::string meshID, std::string textureID, XMVECTOR position, std::string lootType, int quantity) : PhysicsObject(shaderID, meshID, textureID, position) {
	m_lootType = lootType;
	m_quantity = quantity;
}

void Loot::DoCollision(PhysicsObject* other, Map* map) {
	Player* player = other->GetPlayer();
	if (player) {
		map->RemoveObject(this);
		player->Loot(m_lootType, m_quantity);
	}
}

Loot::~Loot() {

}


void Loot::Update(float timeStep) {
	PhysicsObject::Update(timeStep);

	m_boundingBox.SetMin(m_boundingBox.GetMin() + MathsHelper::GetXMVECTOR3(0, 0.25f, 0));
	m_boundingBox.SetMax(m_boundingBox.GetMax() + MathsHelper::GetXMVECTOR3(0, 0.25f, 0));
	Rotate(MathsHelper::GetXMVECTOR3(0, 1.0f * timeStep, 0));
}