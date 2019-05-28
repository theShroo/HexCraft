#pragma once
#include "Hex.h"
#include "Cell.h"

class Cell;
class Map;


class Cluster {
private:
	//cluster tile data stored here.
	std::unordered_map<Hex, Cell*, hash_Hex> m_cluster;

	Hex m_location = Hex{ 0,0,0,0 };
	Map* m_owner = 0;

public:

	Cluster(Hex location, Map* Owner);
	~Cluster();

	Cell* GetCell(Hex cell);
	Map* GetOwner();


};