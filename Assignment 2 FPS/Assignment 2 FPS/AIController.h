#ifndef AICONTROLLER_H
#define AICONTROLLER_H

#include "MathsHelper.h"
#include "GameObjects.h"

class AIPlayer;

class AIController {
public:
	AIController();
	~AIController();
	DirectX::XMVECTOR GetNextTarget(AIPlayer* self);
	void AddPatrolPoint(DirectX::XMVECTOR point);
	int GetType() { return m_type; }
	void SetFollowDistance(float dist) { m_followDistance = dist; }
	float GetFollowDistance() { return m_followDistance; }

private:
	float m_followDistance;
	float m_fireRate;
	std::vector<DirectX::XMVECTOR> m_patrolPoints;
	unsigned m_currentPatrolPoint;
	int m_type = 0;
};



#endif // !AICONTROLLER_H
