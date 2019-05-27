#ifndef SPINNINGOBJECT_H
#define SPINNINGOBJECT_H

#include "GameObjects.h"

class SpinningObject : public GameObject
{
private:
	float m_spinSpeed;

public:
	SpinningObject(DirectX::XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID = "Grey");
	virtual ~SpinningObject();

	float GetSpinSpeed() { return m_spinSpeed; }
	void SetSpinSpeed(float spinSpeed) { m_spinSpeed = spinSpeed; }

	void Update(float timestep);
};

#endif