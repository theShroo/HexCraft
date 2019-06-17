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
	Hex second;
};


class Map {
private:
	int m_clusterSize = 7;

	//TODO localise zones into clusters, then replace with a cluster list.

	// tha actual map data is stored in this object, it gets REALLY big (2gb+)
	std::unordered_map<Hex, Cluster*, hash_Hex> m_map;
	// set an arbitrary cluster size
	//update and render list
	std::unordered_map<PointerKey, Cluster*, PointerHash> m_ActiveClusters;

	// fixed a crash for: iterator overflowing, by scheduling removal after the iterator has finished.
	std::vector<qpair> m_removalQueue;
	std::vector<qpair> m_additionQueue;
	// directions stored as an index to allow for incrementation of a direction to change facing.
	std::vector<Hex> m_directions;
	// improved random number generator. rand() was giving weirdly biased results and skewing item spawn chances and spawn positions
	std::default_random_engine m_generator;
	std::uniform_int_distribution<int> m_spawnchance;
	// function to check if a cell needs to be rendered (ie has exposed faces)
	// a queue for cells to be passed to CheckRender (to avoid invalidating iterators).
	std::vector<Cell*> m_renderCheckQueue;
	// simplex noise for terrain generation;
	SimplexNoise* m_simplexNoise;

public:
	Hex GetDirection(int direction);
	int GetDirection(Hex direction);
	// constructor/destructor
	Map(int clustersize);
	~Map();
	// accessors
	Cell* GetCell(int x, int y, int height);
	Cell* GetCell(Hex cell);
	Cluster* GetCluster(Hex cluster);
	int GetCount();
	void Initialise(Hex position, int distance);
	void GetLocalMap(std::vector<Hex> &localMap, Hex center, int range);
	void GetLocalMap2D(std::vector<Hex> &localMap, Hex center, int range);
	void GetRing(std::vector<Hex> &HexList, Hex center, int radius);
	void GetLine(std::vector<Hex> &HexList, Hex from, Hex to);
	// mutators
	void AddObject(GameObject* object);
	void RemoveObject(GameObject* object);
	GameObject* AddAI(int type, Player* target, float range = 5);
	GameObject* AddLoot(std::string name, std::string shader, std::string mesh, std::string texture, DirectX::XMVECTOR location, int qty);
	// function to check the current update and render list for renderability and updatability.
	void CleanZones(Hex center);
	// a function to add the planes in the direction of travel to the render and update lists.
	void UpdateZones(Hex center, Hex direction, int updateDistance);
	// function to schedule a cell for a render check.
	void RenderCheck(Cell* cell);  
	// a function to increase the render and update distance.
	bool Map::IncrementZone(Hex center, int updateDistance);
	// update and render functions
	void Update(float timetep, DirectX::XMVECTOR center);
	void RenderLocal(DirectX::XMVECTOR location, Direct3D* renderer, Camera* cam);
	std::unordered_map<PointerKey, Cluster*, PointerHash>* GetActiveClusters() { return &m_ActiveClusters; }
		SimplexNoise* simplexNoise();
};
#endif