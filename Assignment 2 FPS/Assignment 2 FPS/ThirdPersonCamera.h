#ifndef THIRD_PERSON_CAMERA_H 
#define THIRD_PERSON_CAMERA_H 

#include "Camera.h" 

#include "GameObjects.h"

class ThirdPersonCamera : public Camera {
private:     
	GameObject* m_objectToFollow;    
	DirectX::XMVECTOR m_offset;
	DirectX::XMVECTOR m_targetOffset;

public:    
	ThirdPersonCamera();     
	ThirdPersonCamera(GameObject* target, DirectX::XMFLOAT3 offset);

			
	void Update(float timestep);

			
	void SetFollowTarget(GameObject* target, DirectX::XMFLOAT3 offset);
	void SetOffset(DirectX::XMFLOAT3 offset);
	void SetOffset(DirectX::XMVECTOR offset);

};

#endif 

