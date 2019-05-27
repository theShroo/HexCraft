#include "Collisions.h"
using namespace DirectX;
using namespace MathsHelper;


namespace Collisions {
	XMVECTOR ClosestPointInBoundingBox(const CBoundingBox& bb, const XMVECTOR& point)
	{
		// refactoring to use vectors is straightforward but very logically complex.
		XMVECTOR result, bbMin = bb.GetMin(), bbMax = bb.GetMax();

		if (result.m128_f32[0] < bbMin.m128_f32[0])
			result.m128_f32[0] = bbMin.m128_f32[0];
		else if (result.m128_f32[0] > bbMax.m128_f32[0])
			result.m128_f32[0] = bbMax.m128_f32[0];

		if (result.m128_f32[1] < bbMin.m128_f32[1])
			result.m128_f32[1] = bbMin.m128_f32[1];
		else if (result.m128_f32[1] > bbMax.m128_f32[1])
			result.m128_f32[1] = bbMax.m128_f32[1];

		if (result.m128_f32[2] < bbMin.m128_f32[2])
			result.m128_f32[2] = bbMin.m128_f32[2];
		else if (result.m128_f32[2] > bbMax.m128_f32[2])
			result.m128_f32[2] = bbMax.m128_f32[2];

		return result;
	}

	bool CheckCollision(const CBoundingSphere& sphere, const XMVECTOR& point)
	{
		return XMVector3Length(sphere.GetCenter() - point).m128_f32[0] <= sphere.GetRadius();
	}
	bool CheckCollision(const CBoundingSphere& sphere1, const CBoundingSphere& sphere2)
	{
		return XMVector3Length(sphere1.GetCenter() - sphere2.GetCenter()).m128_f32[0] <= sphere1.GetRadius() + sphere2.GetRadius();
	}
	bool CheckCollision(const CBoundingSphere& sphere, const CBoundingBox& bb)
	{
		return XMVector3Length(ClosestPointInBoundingBox(bb, sphere.GetCenter()) - sphere.GetCenter()).m128_f32[0] <= sphere.GetRadius();
	}
	bool CheckCollision(const CBoundingBox& bb, const XMVECTOR& point)
	{
		// lets use the vector comparison and comparison result functions here to make this faster.
		uint32_t result1 = XMVector3GreaterOrEqualR(point, bb.GetMin());
		uint32_t result2 = XMVector3GreaterOrEqualR(bb.GetMax(), point);

		return (XMComparisonAllTrue(result1) && XMComparisonAllTrue(result2));
	}
	bool CheckCollision(const CBoundingBox& bb1, const CBoundingBox& bb2)
	{
		uint32_t result1 = XMVector3GreaterOrEqualR(bb1.GetMax(), bb2.GetMin());
		uint32_t result2 = XMVector3GreaterOrEqualR(bb2.GetMax(), bb1.GetMin());

		return (XMComparisonAllTrue(result1) && XMComparisonAllTrue(result2));
	}

	bool CheckCollision(const CBoundingCapsule& bc1, const CBoundingBox& bb) {
		// checks a colision to a box, by getting the closest point on the cylinder and checking if it is inside the box. 
		// center of the box
		XMVECTOR center = (bb.GetMax() + bb.GetMin()) / 2;
		// distance from closest point on line to the box
		float dist = XMVector3LinePointDistance(bc1.GetTop(), bc1.GetBottom(), center).m128_f32[0];
		// create a plane from the line and the center, this gives us the normal for the 
		// plane formed so we can calculate the cross product of this normal and the line segment to get the direction
		// of the closest point from teh bounding box.
		XMVECTOR plane = XMPlaneFromPoints(bc1.GetTop(), bc1.GetBottom(), center);
		XMVECTOR direction = XMVector3Normalize(XMVector3Cross(bc1.GetTop() - bc1.GetBottom(), plane));

		// then we use this closest distance to check if it is inside the bounding box.
		return CheckCollision(bb, center - direction * dist);
	}


	PLANE_RESULT CheckPlane(const CPlane& plane, const XMVECTOR& point)
	{
		float distance = XMVector3Dot(plane.GetNormal(), point).m128_f32[0] + plane.GetDistance();


		if (distance > ON_PLANE_AMOUNT)
			return PLANE_IN_FRONT;
		else if (distance < -ON_PLANE_AMOUNT)
			return PLANE_BEHIND;
		else
			return PLANE_ON;
	}

	PLANE_RESULT CheckPlane(const CPlane& plane, const CBoundingSphere& sphere)
	{
		float distance = XMVector3Dot(plane.GetNormal(), sphere.GetCenter()).m128_f32[0] + plane.GetDistance();

		if (distance > 0 && distance > sphere.GetRadius())
			return PLANE_IN_FRONT;
		else if (distance < 0 && -distance > sphere.GetRadius())
			return PLANE_BEHIND;
		else
			return PLANE_ON;
	}

	PLANE_RESULT CheckPlane(const CPlane& plane, const CBoundingBox& bb)
	{
		XMFLOAT3 diagMin, diagMax, vplaneNormal, bbMin, bbMax;

		XMStoreFloat3(&vplaneNormal, plane.GetNormal());
		XMStoreFloat3(&bbMin, bb.GetMin());
		XMStoreFloat3(&bbMax, bb.GetMin());


		if (vplaneNormal.x >= 0)
		{
			diagMin.x = bbMin.x;
			diagMax.x = bbMax.x;
		}
		else
		{
			diagMin.x = bbMax.x;
			diagMax.x = bbMin.x;
		}

		if (vplaneNormal.y >= 0)
		{
			diagMin.y = bbMin.y;
			diagMax.y = bbMax.y;
		}
		else
		{
			diagMin.y = bbMax.y;
			diagMax.y = bbMin.y;
		}

		if (vplaneNormal.z >= 0)
		{
			diagMin.z = bbMin.z;
			diagMax.z = bbMax.z;
		}
		else
		{
			diagMin.z = bbMax.z;
			diagMax.z = bbMin.z;
		}

		float distanceFromMin = vplaneNormal.x * diagMin.x +
			vplaneNormal.y * diagMin.y +
			vplaneNormal.z * diagMin.z +
			plane.GetDistance();
		if (distanceFromMin > 0.0f)
			return PLANE_IN_FRONT;

		float distanceFromMax = vplaneNormal.x * diagMax.x +
			vplaneNormal.y * diagMax.y +
			vplaneNormal.z * diagMax.z +
			plane.GetDistance();
		if (distanceFromMax < 0.0f)
			return PLANE_BEHIND;

		return PLANE_ON;
	}

	//http://www.cosinekitty.com/raytrace/chapter06_sphere.html
	bool CheckRay(const CRay& ray, const CBoundingSphere& sphere, XMVECTOR* hitPoint)
	{
		XMVECTOR displacement = ray.GetOrigin() - sphere.GetCenter();
		float a = XMVector3LengthSq(ray.GetDirection()).m128_f32[0];
		float b = 2.0f * XMVector3Dot(displacement, ray.GetDirection()).m128_f32[0];
		float c = XMVector3LengthSq(displacement).m128_f32[0] - sphere.GetRadius() * sphere.GetRadius();

		float randicand = b*b - 4.0f * a * c;
		if (randicand < 0.0f)
		{
			return false;
		}


		float root = sqrt(randicand);
		float denom = 2.0f * a;

		float hit1 = (-b + root) / denom;
		float hit2 = (-b - root) / denom;

		if (hit1 < 0 && hit2 < 0)
		{
			return false;
		}


		if (hitPoint)
		{
			*hitPoint = ray.GetDirection() * min(hit1, hit2) + ray.GetOrigin();
		}
		return true;
	}

	//http://tavianator.com/fast-branchless-raybounding-box-intersections/
	bool CheckRay(const CRay& ray, const CBoundingBox& bb, XMVECTOR* hitPoint)
	{

		XMVECTOR bbMin = bb.GetMin(), bbMax = bb.GetMin(), rorigin = ray.GetOrigin(), rdirect = ray.GetDirection();

		float tmin = -INFINITY;
		float tmax = INFINITY;

		float tx1 = (bbMin.m128_f32[0] - rorigin.m128_f32[0]) / rdirect.m128_f32[0];
		float tx2 = (bbMax.m128_f32[0] - rorigin.m128_f32[0]) / rdirect.m128_f32[0];

		tmin = max(tmin, min(tx1, tx2));
		tmax = min(tmax, max(tx1, tx2));

		float ty1 = (bbMin.m128_f32[1] - rorigin.m128_f32[1]) / rdirect.m128_f32[1];
		float ty2 = (bbMax.m128_f32[1] - rorigin.m128_f32[1]) / rdirect.m128_f32[1];

		tmin = max(tmin, min(ty1, ty2));
		tmax = min(tmax, max(ty1, ty2));

		float tz1 = (bbMin.m128_f32[2] - rorigin.m128_f32[2]) / rdirect.m128_f32[2];
		float tz2 = (bbMax.m128_f32[2] - rorigin.m128_f32[2]) / rdirect.m128_f32[2];

		tmin = max(tmin, min(tz1, tz2));
		tmax = min(tmax, max(tz1, tz2));


		if (tmax < 0)
			return false;

		if (tmax >= tmin)
		{
			if (hitPoint)
			{
				float firstHitDistance = tmin < 0 ? tmax : tmin;
				XMVECTOR pointOnBox = (ray.GetDirection()) * firstHitDistance + ray.GetOrigin();
				hitPoint->m128_f32[0] = pointOnBox.m128_f32[0];
				hitPoint->m128_f32[1] = pointOnBox.m128_f32[1];
				hitPoint->m128_f32[2] = pointOnBox.m128_f32[2];
			}
			return true;
		}
		return false;
	}


	bool CheckRay(const CRay& ray, const CPlane& plane, XMVECTOR* hitPoint)
	{
		float planeRayDirection = XMVector3Dot(plane.GetNormal(), ray.GetDirection()).m128_f32[0];

		if (planeRayDirection == 0)
		{
			return false;
		}

		float t = -(XMVector3Dot(plane.GetNormal(), ray.GetOrigin()).m128_f32[0] + plane.GetDistance()) / planeRayDirection;
		if (t < 0)
		{
			return false;
		}
		if (hitPoint)
		{
			XMFLOAT3 pointOnPlane; XMStoreFloat3(&pointOnPlane, (ray.GetDirection() * t) + ray.GetOrigin());
			hitPoint->m128_f32[0] = pointOnPlane.x;
			hitPoint->m128_f32[1] = pointOnPlane.y;
			hitPoint->m128_f32[2] = pointOnPlane.z;
		}
		return true;
	}


	// used from http://geomalgorithms.com/a07-_distance.html#dist3D_Segment_to_Segment()
	float dist3D_Line_to_Line(Line L1, Line L2) {
		XMVECTOR   u = L1.P1 - L1.P0;
		XMVECTOR   v = L2.P1 - L2.P0;
		XMVECTOR   w = L1.P0 - L2.P0;
		float    a = XMVector3Dot(u, u).m128_f32[0];         // always >= 0
		float    b = XMVector3Dot(u, v).m128_f32[0];
		float    c = XMVector3Dot(v, v).m128_f32[0];         // always >= 0
		float    d = XMVector3Dot(u, w).m128_f32[0];
		float    e = XMVector3Dot(v, w).m128_f32[0];
		float    D = a*c - b*b;        // always >= 0
		float    sc, tc;

		// compute the line parameters of the two closest points
		if (D < 0.0000001) {          // the lines are almost parallel
			sc = 0.0;
			tc = (b>c ? d / b : e / c);    // use the largest denominator
		}
		else {
			sc = (b*e - c*d) / D;
			tc = (a*e - b*d) / D;
		}

		// get the difference of the two closest points


		XMVECTOR   dP = w + (sc * u) - (tc * v);  // =  L1(sc) - L2(tc)

		return XMVector3Normalize(dP).m128_f32[0];   // return the closest distance
	}

}