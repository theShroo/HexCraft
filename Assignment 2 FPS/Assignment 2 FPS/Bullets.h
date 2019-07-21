#ifndef BULLETS_H
#define BULLETS_H

#include "PhysicsObject.h"




class Bullets : public PhysicsObject {
public:
	Bullets(std::string shaderID, std::string meshID, std::string textureID, DirectX::XMVECTOR position, DirectX::XMVECTOR velocity, DirectX::XMVECTOR rotation);
	~Bullets();
	static void Initialise(Map* map);
	static Map* m_currentMap;
	virtual void DoCollision(PhysicsObject* other, Map* map);
	virtual void DoCollision(CellPtr other, Map* map);
	virtual void Update(float timeStep);
	virtual PhysicsObject* GetInteractive() { return 0; }
	virtual Player* GetActive() { return 0; }
	virtual Bullets* GetBullet() { return this; }
	float m_ttl;
	bool m_leftGun;
private:
};

class Impact : public GameObject {
public:
	virtual void Update(float timestep);
	Impact(DirectX::XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID = "Grey");
	~Impact();
private:
	float m_ttl;

};



#endif