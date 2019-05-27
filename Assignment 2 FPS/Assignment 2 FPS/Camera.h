/*	FIT2096 - Example Code
*	Camera.h
*	Created by Elliott Wilson - 2015 - Monash University
*	This class represents a Camera in our scene
*	It contains all of the inforamtion needed to create a View and Projection Matrix
*	As well as an Update method which will create the matrices if needed

	this code has had the simplemath library written out in 
	favour of cleaner directx math
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "Direct3D.h"
#include "MathsHelper.h"


class Camera
{
private:
	DirectX::XMVECTOR m_position;			//A view matrix is made up of the position of the camera
	DirectX::XMVECTOR m_lookAtTarget;		//The point in space it is looking at
	DirectX::XMVECTOR m_up;				//And a vector to defines which way is "up" for the camera

	float m_aspectRatio;		//A projection matrix is made up of the aspect ratio of the screen (width/height)
	float m_fieldOfView;		//The field of view of the camera
	float m_nearClip;			//The near clipping plane (anything closer then this to the camera won't be rendered)
	float m_farClip;			//The far clipping plane (anything further then this from the camera won't be rendered)

	DirectX::XMMATRIX m_view;				//This stores our final view matrix
	DirectX::XMMATRIX m_projection;		//This stores our fial projection matrix

	bool m_viewDirty;			//These bools are used to determine if we need to recalculate the View and Projection Matrices
	bool m_projectionDirty;		//Whenever a value is changed these bools are set true, the update method only calculates a matrix if its state is "dirty"

public:
	Camera();	//Constructor
	Camera(DirectX::XMVECTOR pos, DirectX::XMVECTOR lookAt, DirectX::XMVECTOR up, float aspect, float fov, float nearClip, float farClip);	//Parameter Constructor
	~Camera();	//Destructor

				//Mutators
	void SetPosition(DirectX::XMFLOAT3 pos);
	void SetPosition(DirectX::XMVECTOR pos);
	// move function simplifies movement code dramatically
	void Move(DirectX::XMVECTOR displacement);
	void SetLookAt(DirectX::XMFLOAT3 lookAt);
	void SetLookAt(DirectX::XMVECTOR lookAt);
	void SetUp(DirectX::XMFLOAT3 up);

	void SetAspectRatio(float aspect);
	void SetFieldOfView(float fov);
	void SetNearClip(float nearClip);
	void SetFarClip(float farClip);

	//Accessors
	DirectX::XMMATRIX GetView() { return m_view; }
	DirectX::XMMATRIX GetProjection() { return m_projection; }

	DirectX::XMVECTOR GetPosition();

	virtual void Update(float timestep);	//The Update method is used to recalculate the matrices, however later on we could use it to move the camera around
};											//This is why it is virtual and why it receives the timestep as a parameter


#endif