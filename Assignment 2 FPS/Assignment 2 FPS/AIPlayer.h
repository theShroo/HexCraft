#ifndef AISPLAYER_H
#define AISPLAYER_H

#include "Player.h"
#include "AIController.h"


class AIPlayer : public PhysicsObject {
public:
	AIPlayer(DirectX::XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID, int type, float range = 5);
	virtual void Update(float timestep);
	virtual PhysicsObject* GetInteractive() { return this; }
	virtual Player* GetActive() { return 0; }
	virtual AIPlayer* GetAI() { return this; }
	virtual void DoCollision(PhysicsObject* other, Map* map);
	float GetRange() { return m_AI->GetFollowDistance(); }
	void SetTarget(GameObject* target);
	~AIPlayer();
	int GetType() { return m_type; }
	int GetHealth() { return m_health; }
	GameObject* GetTarget() { return m_currentTarget; }
	void AddPatrolPoint(DirectX::XMVECTOR location);
	void SewtFollowDistance(float dist) { m_AI->SetFollowDistance(dist); }
	void SetFireRate(float rof) { m_ROF = 1 / rof; }
private:
	float m_rotationSpeed;
	float m_heading;
	float m_pitch;
	float m_height;
	int m_type;
	AIController* m_AI;
	DirectX::XMVECTOR m_target;
	GameObject* m_currentTarget;
	float m_cooldown;
	float m_ROF;
};
#endif // !FPSPLAYER_H
