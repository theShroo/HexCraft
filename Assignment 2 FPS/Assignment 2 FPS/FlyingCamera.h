#ifndef FLYING_CAMERA_H
#define FLYING_CAMERA_H 

#include "Camera.h"
#include "Input.h" 

class FlyingCamera : public Camera {
private:    
	InputController* m_input;

			
	float m_moveSpeed;   
	float m_rotationSpeed; 
	float m_heightChangeSpeed;

			 
	float m_heading;   
	float m_pitch;

public:   

	FlyingCamera(InputController* input, DirectX::XMFLOAT3 startPos);
	void Update(float timestep);
};

#endif 

