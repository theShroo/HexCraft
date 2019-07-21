
#ifndef MAP_H
#define MAP_H

#include "Cluster.h"
#include "Collisions.h"
#include "SimplexNoise.h"

// map object declaration. the properties of hex cell co-ordinates and their ability to be hashed uniquely allows us to use a hash table instead of multi-dimensional vectors.
// this also allows our maps to become irregular in shape and to contain "holes", without wasting memory storing empty cells.
// we use references to cells so that in the case that the table is resized then the referenced cells retain their validity.

// pair type to store data for addition and removal queues
struct qpair {
	GameObject* first;
	HexLogic::Hex second;
};


class Map {
private:
	// set an arbitrary cluster size
	int m_clusterSize = 7;
	std::unordered_map<HexLogic::Hex, Cluster*, HexLogic::hash_Hex> m_map;
	std::unordered_map<PointerKey, Cluster*, PointerHash> m_ActiveClusters;

	// fixed a crash for: iterator overflowing, by scheduling removal after the iterator has finished.
	std::vector<qpair> m_removalQueue;
	std::vector<qpair> m_additionQueue;
	std::vector<CellPtr> m_renderCheckQueue;
	// directions stored as an index to allow for incrementation of a direction to change facing.
	std::vector<HexLogic::Hex> m_directions;
	// random number and simplex noise elements for map generation
	std::default_random_engine m_generator;
	std::uniform_int_distribution<int> m_spawnchance;
	SimplexNoise* m_simplexNoise;

public:
	// constructor/destructor/initialisers
	Map(int clustersize, HexLogic::Hex position, int distance);
    ~Map();


	// Getters
	HexLogic::Hex GetDirection(int direction);
	int GetDirection(HexLogic::Hex direction);
	SimplexNoise* simplexNoise();



	// accessors
	
	// these accessors create the object if none exists;
	CellPtr GetCell(int x, int y, int height);
	CellPtr GetCell(HexLogic::Hex cell);
	Cluster* GetCluster(HexLogic::Hex cluster);
	int GetCount();
	std::unordered_map<PointerKey, Cluster*, PointerHash>* GetActiveClusters() { return &m_ActiveClusters; }
	void GetLocalMap(std::vector<HexLogic::Hex> &localMap, HexLogic::Hex center, int range);
	void GetLocalMap2D(std::vector<HexLogic::Hex> &localMap, HexLogic::Hex center, int range);
	void GetRing(std::vector<HexLogic::Hex> &HexList, HexLogic::Hex center, int radius);
	void GetLine(std::vector<HexLogic::Hex> &HexList, HexLogic::Hex from, HexLogic::Hex to);



	// mutators
	void AddObject(GameObject* object);
	void RemoveObject(GameObject* object);
	GameObject* AddAI(int type, Player* target, float range = 5);
	GameObject* AddLoot(std::string name, std::string shader, std::string mesh, std::string texture, DirectX::XMVECTOR location, int qty);
	
	// function to check the current update and render list for renderability and updatability.
	void CleanZones(HexLogic::Hex center);
	// a function to add the planes in the direction of travel to the render and update lists.
	void UpdateZones(HexLogic::Hex center, HexLogic::Hex direction, int updateDistance);
	// function to schedule a cell for a render check.
	void RenderCheck(HexLogic::Hex cell);
	// a function to increase the render and update distance.
	bool Map::IncrementZone(HexLogic::Hex center, int updateDistance);
	// update and render functions
	void Update(float timetep, DirectX::XMVECTOR center);
	void RenderLocal(DirectX::XMVECTOR location, Direct3D* renderer, Camera* cam);
};

#endif