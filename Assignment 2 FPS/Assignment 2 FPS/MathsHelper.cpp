#include "MathsHelper.h"

using namespace::DirectX;

namespace MathsHelper {
	float RandomRange(float min, float max)
	{
		return min + (max - min) * ((float)rand() / RAND_MAX);
	}

	int RandomRange(int min, int max)
	{
		// Includes min and max
		return rand() % (max - min + 1) + min;
	}

	float RemapRange(float value, float fromMin, float fromMax, float toMin, float toMax)
	{
		// Example - remap 10 from range 0-20 to range 0-100. Result is 50
		return (((value - fromMin) * (toMax - toMin)) / (fromMax - fromMin)) + toMin;
	}

	// Amount of 0 will result in value1, amount of 1 will result in value 2
	float LerpFloat(float value1, float value2, float amount)
	{
		return value1 + (value2 - value1) * amount;
	}

	float Clamp(float value, float min, float max)
	{
		if (value > max)
			return max;
		else if (value < min)
			return min;
		else
			return value;
	}
	XMFLOAT3 Zero() {
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	XMVECTOR ZeroVector3() {

		XMFLOAT3 temp = XMFLOAT3(0.0f, 0.0f, 0.0f);
		return XMLoadFloat3(&temp);
	}
	XMVECTOR UpVector3() {

		XMFLOAT3 temp = XMFLOAT3(0.0f, 1.0f, 0.0f);
		return XMLoadFloat3(&temp);
	}
	XMVECTOR ForwardVector3() {

		XMFLOAT3 temp = XMFLOAT3(0.0f, 0.0f, 1.0f);
		return XMLoadFloat3(&temp);
	}

	XMVECTOR RightVector3() {

		XMFLOAT3 temp = XMFLOAT3(1.0f, 0.0f, 0.0f);
		return XMLoadFloat3(&temp);
	}

	XMVECTOR GetXMVECTOR4(float a, float b, float c, float d) {
		XMFLOAT4 temp = XMFLOAT4(a, b, c, d);
		return XMLoadFloat4(&temp);
	}
	XMVECTOR GetXMVECTOR4(XMFLOAT4 a) {
		return XMLoadFloat4(&a);
	}

	XMVECTOR GetXMVECTOR3(float a, float b, float c) {
		XMFLOAT3 temp = XMFLOAT3(a, b, c);
		return XMLoadFloat3(&temp);
	}

	XMVECTOR GetXMVECTOR3(XMFLOAT3 a) {
		return XMLoadFloat3(&a);
	}

	XMVECTOR GetXMVECTOR2(float a, float b) {
		XMFLOAT2 temp = XMFLOAT2(a, b);
		return XMLoadFloat2(&temp);
	}
	XMVECTOR GetXMVECTOR2(XMFLOAT2 a) {
		return XMLoadFloat2(&a);
	}
	XMVECTOR GetXMVECTOR1(float a) {
		return XMLoadFloat(&a);
	}
	wchar_t* ConvertString(const char * str)
	{
		// Convert C string (which we like using) into a wchar_t* (which the texture loader likes) 
		// https://msdn.microsoft.com/en-us/library/ms235631.aspx
		size_t newsize = strlen(str) + 1;
		wchar_t* wcstring = new wchar_t[newsize];
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wcstring, newsize, str, _TRUNCATE);
		return wcstring;
	}

	float Distance(XMFLOAT3 pointa, XMFLOAT3 pointb) {
		float distance;
		XMStoreFloat(&distance, XMVector3Length(GetXMVECTOR3(pointa) - GetXMVECTOR3(pointb)));
		return distance;
	}

	float Dot(XMFLOAT3 pointa, XMFLOAT3 pointb) {
		float dot;
		XMStoreFloat(&dot, XMVector3Dot(GetXMVECTOR3(pointa), GetXMVECTOR3(pointb)));
		return dot;
	}
	float Dot(XMVECTOR pointa, XMFLOAT3 pointb) {
		float dot;
		XMStoreFloat(&dot, XMVector3Dot(pointa, GetXMVECTOR3(pointb)));
		return dot;
	}

	float GetLength(XMVECTOR vector) {
		float length;
		XMStoreFloat(&length, XMVector3Length(vector));
		return length;
	}


};