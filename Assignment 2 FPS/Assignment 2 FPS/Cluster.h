#pragma once
#include "Hex.h"

class Cell;
class Map;


class Cluster {
private:
	//cluster tile data stored here.
	std::unordered_map<HexLogic::Hex, CellPtr, HexLogic::hash_Hex> m_cluster;
	std::unordered_map<PointerKey, CellPtr, PointerHash> m_cellRenderables;
	std::unordered_map<PointerKey, CellPtr, PointerHash> m_cellUpdatables;
	std::unordered_map<PointerKey, GameObject*, PointerHash> m_entities;

	int m_clustersize;
	HexLogic::Hex m_b_location = HexLogic::Hex{ 0,0,0,0 };
	Map* m_owner = 0;
	bool m_initialised = 0;
	void _Initialise();

	//void _Deinitialise();

public:

	Cluster(HexLogic::Hex location, Map* Owner, int clustersize);
	~Cluster();

	operator PointerKey() { return PointerKey{ reinterpret_cast<size_t>(this) }; }

	CellPtr GetCell(HexLogic::Hex cell);
	Map* GetOwner();
	int GetCount();
	// CellPtr CheckCell(HexLogic::Hex cell);
	void Update(float timestep, std::vector<GameObject*> &entitiesToUpdate, DirectX::XMVECTOR center);
	void DisableUpdate(CellPtr cell);
	void EnableUpdate(CellPtr cell);
	void DisableRender(CellPtr cell);
	void EnableRender(CellPtr cell);
	void Render(Direct3D* renderer, Camera* cam);
	std::unordered_map<PointerKey, GameObject*, PointerHash>* GetEntities();
	void Clean(HexLogic::Hex center);
	HexLogic::Hex GetLocation() { return m_b_location; };
};