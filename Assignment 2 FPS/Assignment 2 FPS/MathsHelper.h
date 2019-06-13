#ifndef MATHS_HELPER_H
#define MATHS_HELPER_H

#include <DirectXMath.h>
#include <random>
#include <unordered_map>
#include <random>
#include <chrono>
#include <iostream>
#include <ctime>
#include <vector>
#include <sstream>
#include <functional>


#define PI 3.14159f
#define ToRadians(degree) ((degree) * (PI / 180.0f))
#define ToDegrees(radian) ((radian * 180.0f) / (PI))

struct PointerKey {
	size_t value;

	bool operator==(const PointerKey &other) const {
		return (value == other.value);
	}
};

struct PointerHash {
	size_t operator()(const PointerKey &key) const {
		size_t x = key.value;
		x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
		x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
		x = x ^ (x >> 31);
		return x;
	}

};

class Texture;
class Direct3D;
class GameObject;
class Camera;
class Player;
class Equipment;
class AIPlayer;
class PhysicsObject;
class Bullets;
class Loot;
class Cell;
class Map;



namespace MathsHelper{
	float RandomRange(float min, float max);
	int RandomRange(int min, int max);
	int RoundFloatToInt(float value);
	float RemapRange(float value, float fromMin, float fromMax, float toMin, float toMax);

	float LerpFloat(float value1, float value2, float amount);
	float Clamp(float value, float min, float max);
	DirectX::XMFLOAT3 Zero();
	DirectX::XMVECTOR ZeroVector3();
	DirectX::XMVECTOR UpVector3();
	DirectX::XMVECTOR ForwardVector3();
	DirectX::XMVECTOR RightVector3();
	DirectX::XMVECTOR GetXMVECTOR4(float a, float b, float c, float d);
	DirectX::XMVECTOR GetXMVECTOR4(DirectX::XMFLOAT4 a);
	DirectX::XMVECTOR GetXMVECTOR3(float a, float b, float c);
	DirectX::XMVECTOR GetXMVECTOR3(DirectX::XMFLOAT3 a);
	DirectX::XMVECTOR GetXMVECTOR2(float a, float b);
	DirectX::XMVECTOR GetXMVECTOR2(DirectX::XMFLOAT2 a);
	DirectX::XMVECTOR GetXMVECTOR1(float a);
	float Distance(DirectX::XMFLOAT3 pointa, DirectX::XMFLOAT3 pointb);
	float Dot(DirectX::XMFLOAT3 pointa, DirectX::XMFLOAT3 pointb);
	float Dot(DirectX::XMVECTOR pointa, DirectX::XMFLOAT3 pointb);
	float GetLength(DirectX::XMVECTOR vector);
	// holy shit batman, this function contains a memory leak! this may take some refactoring to fix...
	wchar_t* ConvertString(const char * str);


};


#endif