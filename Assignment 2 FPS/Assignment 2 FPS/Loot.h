#ifndef LOOT_H
#define LOOT_H
#include "PhysicsObject.h"


class Map;
class Loot : public PhysicsObject {
public:

	Loot(std::string shaderID, std::string meshID, std::string textureID, DirectX::XMVECTOR position, std::string lootType, int quantity = 1);
	~Loot();
	virtual void DoCollision(PhysicsObject* other, Map* map);
	virtual Loot* GetLoot() { return this; }
	virtual void Update(float timeStep);
	virtual PhysicsObject* GetInteractive() { return 0; }
	std::string m_lootType;
	int m_quantity;
};
#endif