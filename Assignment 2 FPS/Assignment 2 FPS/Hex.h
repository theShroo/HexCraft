#ifndef HEX_H
#define HEX_H
#include <functional>
#include "Direct3D.h"

// struct to describe a hex tiles co-ordinates, each tile has a unique triplet.
// we are going to extend this class to incorporate a height identifier, using a 4th int 
// and hashing it with the rest.
using namespace DirectX;

struct Hex {
	int x, y, z, w;

	// we also need to overload the equality  and inequality operators to provide a method for comparing quantitativly that two hexes are the same.
	bool operator==(const Hex& other) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}
	bool operator!=(const Hex& other) const
	{
		return !(this->operator==(other));
	}

	// operator- is used to get a relative location, like a vector.
	Hex operator-(const Hex& other) {
		return Hex{ x - other.x, y - other.y, z - other.z, w - other.w };
	}

	// and here we overload the addition (+) operator to allow us to find a hex offset from a given hex.
	Hex operator+(const Hex& other) {
		return Hex{ other.x + x, other.y + y, other.z + z, other.w + w };
	}
	// here we will also overload the += operator to allow this functionality to apply to Hex.
	Hex operator+=(const Hex& other) {
		x = x + other.x;
		y = y + other.y;
		z = z + other.z;
		z = w + other.w;
		return *this;
	}
	// lets also permit the multiplication of a hex by a scalar
	Hex operator*(const int scalar) {
		return Hex{ x * scalar, y * scalar, z * scalar , w * scalar };
	}

	// here we add a function to provide a distance between two hexes.
	int distance(const Hex& other) {
		return ((abs(x - other.x) + abs(y - other.y) + abs(z - other.z)) / 2);
	}

	static XMVECTOR HexToVector(Hex coordinates) {
		return MathsHelper::GetXMVECTOR3(1.732f * (coordinates.x + (float)coordinates.y / 2), coordinates.w, (float)3 / 2 * coordinates.y);
	}

	static Hex Float4ToHex(XMFLOAT4 coordinates) {

		float rx = round(coordinates.x);
		float ry = round(coordinates.y);
		float rz = round(coordinates.z);

		float x_diff = abs(rx - coordinates.x);
		float y_diff = abs(ry - coordinates.y);
		float z_diff = abs(rz - coordinates.z);

		if (x_diff > y_diff && x_diff > z_diff) {
			rx = -ry - rz;
		}
		else if (y_diff > z_diff) {
			ry = -rx - rz;
		}
		else {
			rz = -rx - ry;
		}
		return Hex{ int(rx), int(ry), int(rz), int(round(coordinates.w)) };
	}

	static Hex VectorToHex(XMVECTOR coordinates) {
		// this conversion code gets a little messy, but without it the getlocal function (the most noticable of the functions that use this conversion)
		// would all generate a positional bias as the distcance from 0 increases
		// this function also converts the XMVECTOR coordinates to hex format, but leaves the floating point data intact for the xmfloat to hex function to round correctly


		XMFLOAT4 coords;
		XMStoreFloat4(&coords, coordinates);
		coords.x = (coords.x * 1.732f / 3) - (coords.z / 3);
		coords.z /= 1.5f;
		return Float4ToHex(XMFLOAT4(coords.x, coords.z, -coords.x - coords.z, coords.y));
	}

	static XMFLOAT4 LerpHex(Hex a, Hex b, float t) {
		XMFLOAT4 result(MathsHelper::LerpFloat(a.x, b.x, t),
			MathsHelper::LerpFloat(a.y, b.y, t),
			MathsHelper::LerpFloat(a.z, b.z, t),
			MathsHelper::LerpFloat(a.w, b.w, t));
		return result;
	}

	static Hex smalltobig(Hex smallHex, int radius) {
		int x = smallHex.x, y = smallHex.y, z = smallHex.z;
		double area = (3 * pow(radius, 2)) + (3 * radius) + 1;
		float shift = (3 * radius) + 2;
		float xh = (y + shift * x) / area;
		float yh = (z + shift * y) / area;
		float zh = (x + shift * z) / area;
		int i = (1 + xh - yh) / 3;
		int j = (1 + yh - zh) / 3;
		int k = (1 + zh - xh) / 3;
		int l = smallHex.w / 15;
		return Hex{ i, j, k, l };
	}

	// testing phase, single test case okay!
	static Hex bigtosmall(Hex BigHex, int radius) {
		int x = BigHex.x, y = BigHex.y, z = BigHex.z;
		int i = (x*((radius*2) +1))+ (y*radius);
		int j = (y*(radius+1)) - (x*radius);
		int k = -i - j;
		int l = BigHex.w * 15;
		return Hex{ i, j, k, l };
	}
};

// this function takes Hex data as an xmfloat4 and rounds it to the nearest hex.



struct hash_Hex {
	// this unique triplet allows us to create a unique hash for each tile by using the following hash function. this function overloads the () operator to provide a hashable value when called as a function
	size_t operator()(const Hex &hex) const {
		return std::hash<int>()(hex.x) ^ std::hash<int>()(hex.y) ^ std::hash<int>()(hex.z) ^ std::hash<int>()(hex.w);
	}

};



#endif // !Hex-H
