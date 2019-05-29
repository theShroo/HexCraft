#pragma once
#include "Hex.h"
#include "Cell.h"

class Cell;
class Map;


class Cluster {
private:
	//cluster tile data stored here.
	std::unordered_map<Hex, Cell*, hash_Hex> m_cluster;
	std::unordered_map<PointerKey, Cell*, PointerHash> m_cellRenderables;
	std::unordered_map<PointerKey, Cell*, PointerHash> m_cellUpdatables;


	Hex m_location = Hex{ 0,0,0,0 };
	Map* m_owner = 0;


public:

	Cluster(Hex location, Map* Owner);
	~Cluster();
	operator PointerKey() { return PointerKey{ reinterpret_cast<size_t>(this) }; }

	Cell* GetCell(Hex cell);
	Map* GetOwner();
	void Update(float timestep, std::vector<GameObject*> &movedZone, std::vector<GameObject*> &entitiesToUpdate);
	void DisableUpdate(Cell* cell);
	void EnableUpdate(Cell* cell);
	void DisableRender(Cell* cell);
	void EnableRender(Cell* cell);
	void Render(Direct3D* renderer, Camera* cam);
	void Clean(Hex center);
	Hex GetLocation() { return m_location; };
};