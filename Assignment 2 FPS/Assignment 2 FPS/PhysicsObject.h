#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H 

#include "GameObjects.h" 
#include "Collisions.h"

class Map;

class PhysicsObject : public GameObject {
protected:     
	DirectX::XMVECTOR m_velocity;  
	DirectX::XMVECTOR m_acceleration;
	int m_health;
	CBoundingBox m_boundingBox;

	float m_frictionAmount;

	void ApplyFriction(float strength);

public:    
	PhysicsObject(std::string shaderID, std::string meshID, std::string textureID, DirectX::XMVECTOR position);
	virtual void Update(float timestep);
	// a few quick safe conversions from the base class to the parent class.
	virtual PhysicsObject* GetInteractive() { return this; }
	virtual Player* GetActive() { return 0; }
	virtual Bullets* GetBullet() { return 0; }
	virtual AIPlayer* GetAI() { return 0; }
	virtual Player* GetPlayer() { return 0; }
	virtual Loot* GetLoot() { return 0; }
	virtual void SetMesh(std::string meshID, unsigned meshIndex = 0);
	void DoStanding(float surface, Map* map);

	DirectX::XMVECTOR GetVelocity() { return m_velocity; }
	CBoundingBox& GetBounds() { return m_boundingBox; };
	void ApplyForce(DirectX::XMVECTOR force);
	virtual void DoCollision(PhysicsObject* other, Map* map);
	void Damage(int damageDone) { m_health -= damageDone; }
	int GetHealth() { return m_health; }
	~PhysicsObject();
};

#endif