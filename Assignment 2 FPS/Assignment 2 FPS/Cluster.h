#pragma once
#include "Hex.h"

class Cell;
class Map;


class Cluster {
private:
	//cluster tile data stored here.
	std::unordered_map<Hex, Cell*, hash_Hex> m_cluster;
	std::unordered_map<PointerKey, Cell*, PointerHash> m_cellRenderables;
	std::unordered_map<PointerKey, Cell*, PointerHash> m_cellUpdatables;
	std::unordered_map<PointerKey, GameObject*, PointerHash> m_entities;

	int m_clustersize;
	Hex m_location = Hex{ 0,0,0,0 };
	Map* m_owner = 0;
	bool m_initialised = 0;
	void _Initialise();

public:

	Cluster(Hex location, Map* Owner, int clustersize);
	~Cluster();

	operator PointerKey() { return PointerKey{ reinterpret_cast<size_t>(this) }; }

	Cell* GetCell(Hex cell);
	Map* GetOwner();
	int GetCount();
	void Update(float timestep, std::vector<GameObject*> &entitiesToUpdate, XMVECTOR center);
	void DisableUpdate(Cell* cell);
	void EnableUpdate(Cell* cell);
	void DisableRender(Cell* cell);
	void EnableRender(Cell* cell);
	void Render(Direct3D* renderer, Camera* cam);
	std::unordered_map<PointerKey, GameObject*, PointerHash>* GetEntities();
	void Clean(Hex center);
	Hex GetLocation() { return m_location; };
};