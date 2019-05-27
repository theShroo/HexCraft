#ifndef HEX_H
#define HEX_H
#include <functional>

// struct to describe a hex tiles co-ordinates, each tile has a unique triplet.
// we are going to extend this class to incorporate a height identifier, using a 4th int 
// and hashing it with the rest.

struct Hex {
	int x, y, z, w;

	// we also need to overload the equality  and inequality operators to provide a method for comparing quantitativly that two hexes are the same.
	bool operator==(const Hex &other) const
	{
		return (x == other.x && y == other.y && z == other.z && w == other.w);
	}
	bool operator!=(const Hex &other) const
	{
		return !(this->operator==(other));
	}

	// operator- is used to get a relative location, like a vector.
	Hex operator-(const Hex &other) {
		return Hex{ x - other.x, y - other.y, z - other.z, w - other.w };
	}

	// and here we overload the addition (+) operator to allow us to find a hex offset from a given hex.
	Hex operator+(const Hex &other) {
		return Hex{ other.x + x, other.y + y, other.z + z, other.w + w };
	}
	// here we will also overload the += operator to allow this functionality to apply to Hex.
	Hex operator+=(const Hex &other) {
		x = x + other.x;
		y = y + other.y;
		z = z + other.z;
		z = w + other.w;
		return *this;
	}
	// lets also permit the multiplication of a hex by a scalar
	Hex operator*(const int scalar) {
		return Hex{ x*scalar, y*scalar, z*scalar , w*scalar};
	}

	// here we add a function to provide a distance between two hexes.
	int distance(const Hex &other) {
		return ((abs(x - other.x) + abs(y - other.y) + abs(z - other.z)) / 2);
	}
};



struct hash_Hex {
	// this unique triplet allows us to create a unique hash for each tile by using the following hash function. this function overloads the () operator to provide a hashable value when called as a function
	size_t operator()(const Hex &hex) const {
		return std::hash<int>()(hex.x) ^ std::hash<int>()(hex.y) ^ std::hash<int>()(hex.z) ^ std::hash<int>()(hex.w);
	}

};

#endif // !Hex-H
