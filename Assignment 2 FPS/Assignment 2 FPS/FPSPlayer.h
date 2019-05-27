#ifndef FPSPLAYER_H
#define FPSPLAYER_H

#include "Player.h"
#include "Camera.h"

class FPSPlayer : public Player{
public:
	FPSPlayer(DirectX::XMVECTOR position, std::string shaderID, std::string meshID, std::string textureID, InputController* input);
	virtual void Update(float timestep);
	virtual PhysicsObject* GetInteractive() { return this; }
	virtual Player* GetActive() { return this; }
	virtual void Render(Direct3D* renderer, Camera* cam);
	~FPSPlayer();
	Camera* GetCamera();
private:
	Camera* m_camera;
	float m_rotationSpeed;
	float m_heading;
	float m_pitch;
	float m_height;
	InputController* m_input;
};
#endif // !FPSPLAYER_H
