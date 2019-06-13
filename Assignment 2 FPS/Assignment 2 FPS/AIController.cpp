

#include "AIController.h"
#include "AIPlayer.h"

using namespace DirectX;

AIController::AIController() {
	m_currentPatrolPoint = 0;
	m_fireRate = 3;
	m_followDistance = 5;

}
void AIController::AddPatrolPoint(XMVECTOR point) {
	m_patrolPoints.push_back(point);
}


AIController::~AIController() {


}

XMVECTOR AIController::GetNextTarget(AIPlayer *self) {
	XMVECTOR result;
	if (m_patrolPoints.size() > 0) {
		result = m_patrolPoints[m_currentPatrolPoint];
		m_currentPatrolPoint++;
		if (m_currentPatrolPoint > m_patrolPoints.size()) {
			m_currentPatrolPoint = 0;
		}
	}
	else if (self->GetType() == 2){
		result = self->GetPosition() + MathsHelper::GetXMVECTOR3(float(rand() % 10 - 5), 0.0f, float(rand() % 10 - 5));
	}
	else if (self->GetTarget()) {
		XMVECTOR target = self->GetTarget()->GetPosition();
		XMVECTOR direction = target - self->GetPosition();
		float dist;
		XMStoreFloat(&dist, XMVector3Length(direction));
		if (dist > m_followDistance) {
			result = self->GetPosition() + (direction * ((dist - m_followDistance) / dist));
		}
		else if (dist < m_followDistance * 0.8f) {
			result = self->GetPosition() + (direction *  m_followDistance);

		}

		else {
			result = self->GetPosition();
		}
	}
	else {
		result = self->GetPosition();
	}

	return result;
}
