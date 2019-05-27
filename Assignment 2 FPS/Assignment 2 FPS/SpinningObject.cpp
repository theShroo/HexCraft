#include "SpinningObject.h"
using namespace DirectX;

SpinningObject::SpinningObject(XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID) : GameObject(position, shaderID, meshID, textureID)
{
	
	m_spinSpeed = 1.0f;
}

SpinningObject::~SpinningObject() { }

void SpinningObject::Update(float timestep)
{
	// Constantly spin around the Y axis
	Rotate(MathsHelper::GetXMVECTOR3(0,m_spinSpeed * timestep,0));
}