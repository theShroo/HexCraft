#ifndef PLAYER_H
#define PLAYER_H

#include "PhysicsObject.h"
#include "Input.h"
#include "Equipment.h"

class FPSPlayer;

class Player : public PhysicsObject
{
private:
	InputController* m_input;

	float m_moveSpeed;
	float m_turnSpeed;
	float m_distance_traveled;
	float m_cooldown;
	std::wstring m_ammoCount;
	std::unordered_map <PointerKey, std::pair<Equipment*, int>, PointerHash> m_inventory;
	EquipmentList m_equiped_gear;
	CBoundingCapsule m_capsule;

public:
	Player(DirectX::XMVECTOR position, std::string shader, std::string mesh, std::string texture, InputController* input);
	float GetDistance();
	int GetHealth() { return m_health; }
	virtual void Update(float timestep);
	virtual PhysicsObject* GetInteractive() { return this; }
	virtual Player* GetActive() { return this; }
	virtual Player* GetPlayer() { return this; }
	void Loot(std::string itemName, int quantity);
	int GetItemCount(std::string itemName);
	std::wstring& GetAmmoCounter() { return m_ammoCount; }
	friend FPSPlayer;
	void Equip(std::string item);
	void Unequip(int index);

	std::string GetEquipmentDescription(unsigned index);
	void DoAttack(DirectX::XMVECTOR direction);
	CBoundingCapsule& GetBounds() { return m_capsule; }
	CBoundingBox& GetBBBounds() { return m_boundingBox; }
	std::unordered_map <PointerKey, std::pair<Equipment*, int>, PointerHash>* GetInventory() { return &m_inventory; }

};

#endif