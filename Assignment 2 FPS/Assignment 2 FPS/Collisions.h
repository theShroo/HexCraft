#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "MathsHelper.h"

#define ON_PLANE_AMOUNT 0.001f

enum PLANE_RESULT
{
	PLANE_IN_FRONT = 0,
	PLANE_ON,
	PLANE_BEHIND
};
struct Line {
	DirectX::XMVECTOR P0;
	DirectX::XMVECTOR P1;
};
class CBoundingSphere
{
private:
	DirectX::XMVECTOR m_center;
	float m_radius;

public:
	CBoundingSphere() { m_center = MathsHelper::ZeroVector3(); m_radius = 0; }
	CBoundingSphere(DirectX::XMVECTOR center, float radius) { m_center = center; m_radius = radius; }

	DirectX::XMVECTOR GetCenter() const { return m_center; }
	float GetRadius() const { return m_radius; }

	void SetCenter(DirectX::XMVECTOR center) { m_center = center; }
	void SetRadius(float radius) { m_radius = radius; }
};

class CBoundingBox
{
private:
	DirectX::XMVECTOR m_min;
	DirectX::XMVECTOR m_max;

public:
	CBoundingBox() { m_min = MathsHelper::ZeroVector3(); m_max = MathsHelper::ZeroVector3(); }
	CBoundingBox(DirectX::XMVECTOR min, DirectX::XMVECTOR max) { m_min = min; m_max = max; }

	DirectX::XMVECTOR GetMin() const { return m_min; }
	DirectX::XMVECTOR GetMax() const { return m_max; }

	void SetMin(DirectX::XMVECTOR min) { m_min = min; }
	void SetMax(DirectX::XMVECTOR max) { m_max = max; }
};

class CBoundingCapsule {
private:
	DirectX::XMVECTOR m_top;
	DirectX::XMVECTOR m_bottom;
	float m_radius;

public:
	CBoundingCapsule() { m_top = MathsHelper::ZeroVector3(); m_bottom = MathsHelper::ZeroVector3(); m_radius = 0; }
	CBoundingCapsule(DirectX::XMVECTOR top, DirectX::XMVECTOR bottom, float radius) { m_top = top; m_bottom = bottom; m_radius = radius; }

	DirectX::XMVECTOR GetTop() const { return m_top; }
	DirectX::XMVECTOR GetBottom() const { return m_bottom; }
	float GetRadius() const { return m_radius; }

	void SetTop(DirectX::XMVECTOR top) { m_top = top; }
	void SetBottom(DirectX::XMVECTOR bottom) { m_bottom = bottom; }
	void SetRadius(float radius) { m_radius = radius; }

};

class CPlane
{
private:
	DirectX::XMVECTOR m_normal;
	float m_distance;

public:
	CPlane() { m_normal = MathsHelper::ZeroVector3(); m_distance = 0; }
	CPlane(DirectX::XMVECTOR normal, float distance) { m_normal = normal; m_distance = distance; }

	DirectX::XMVECTOR GetNormal() const { return m_normal; }
	float GetDistance() const { return m_distance; }

	void SetNormal(DirectX::XMVECTOR normal) { m_normal = normal; }
	void SetDistance(float distance) { m_distance = distance; }

	void Verify() { m_normal = DirectX::XMVector3Normalize(m_normal); }
};

class CRay
{
private:
	DirectX::XMVECTOR m_origin;
	DirectX::XMVECTOR m_direction;

public:
	CRay() { m_origin = MathsHelper::ZeroVector3(); m_direction = MathsHelper::ZeroVector3(); }
	CRay(DirectX::XMVECTOR origin, DirectX::XMVECTOR direction) { m_origin = origin; m_direction = direction; }

	DirectX::XMVECTOR GetOrigin() const { return m_origin; }
	DirectX::XMVECTOR GetDirection() const { return m_direction; }

	void SetOrigin(DirectX::XMVECTOR origin) { m_origin = origin; }
	void SetDirection(DirectX::XMVECTOR direction) { m_direction = direction; }

	void Verify() { m_direction = DirectX::XMVector3Normalize(m_direction); }
};


namespace Collisions {



	bool CheckCollision(const CBoundingSphere& sphere, const DirectX::XMVECTOR& point);
	bool CheckCollision(const CBoundingSphere& sphere1, const CBoundingSphere& sphere2);
	bool CheckCollision(const CBoundingSphere& sphere, const CBoundingBox& bb);
	bool CheckCollision(const CBoundingBox& bb, const DirectX::XMVECTOR& point);
	bool CheckCollision(const CBoundingBox& bb1, const CBoundingBox& bb2);
	bool CheckCollision(const CBoundingCapsule& bc1, const CBoundingBox& bb2);


	PLANE_RESULT CheckPlane(const CPlane& plane, const DirectX::XMVECTOR& point);
	PLANE_RESULT CheckPlane(const CPlane& plane, const CBoundingSphere& sphere);
	PLANE_RESULT CheckPlane(const CPlane& plane, const CBoundingBox& bb);

	bool CheckRay(const CRay& ray, const CBoundingSphere& sphere, DirectX::XMVECTOR* hitPoint);
	bool CheckRay(const CRay& ray, const CBoundingBox& bb, DirectX::XMVECTOR* hitPoint);
	bool CheckRay(const CRay& ray, const CPlane& plane, DirectX::XMVECTOR* hitPoint);

	float dist3D_Line_to_Line(Line u, Line v);
}
#endif