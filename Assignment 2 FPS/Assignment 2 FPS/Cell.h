// to reduce memory overheads the cell class will be reduced to a struct.

/*	Cell.h written by Russell Chidambaranathan 2017.
*
*	a class to describe the Cell object.
*	contains all the methods used by the map to manipulate cells.
*/

#ifndef CELL_H
#define CELL_H
#include "MathsHelper.h"
#include "Hex.h"


// Cell struct, describes the contents of each cell. location is not required by the cell, so no hex is stored.

class Cell {

private:
	WeakCellPtr* (Cell::*m_initialised)();
	WeakCellPtr* _GetNeigbours();
	WeakCellPtr* _GetNeigboursInit();
	WeakCellPtr m_neighbours[20];  // notable locations: index 9 is the cell below, 10 is above.
	bool m_passable;
	bool m_solid;
	int m_health;
	int m_resistance;
	void Initialise();
	HexLogic::Hex m_location;
	// initialise the maps to the entities that exist within the area of this cell.
	// these use the objects pointer as a key to reduce lookup times to O(1)
	std::string m_type;
	GameObject* m_gameObject;
	DirectX::XMVECTOR m_position;
	Cluster* m_cluster;
	Map* m_owner;
public:
	
	Cell(DirectX::XMVECTOR location, Cluster* owner);
	void Render(Direct3D* renderer, Camera* cam);
	void SetType(std::string new_type, bool passable, bool solid);
	std::string GetType();
	void Update(float timestep);
	~Cell();
	bool IsPassable() { return m_passable; }
	void Break(int damage, int penetration);
	void SetResistance(int resistance) { m_resistance = resistance; }
	virtual WeakCellPtr* GetNeigbours();
	bool IsSolid() { return m_solid; }
	// to improve performance the list of updateables and renderables needs to identify items by their pointers.
	operator PointerKey() { return PointerKey{ reinterpret_cast<size_t>(this) }; }
	HexLogic::Hex GetLocation();
	Cluster* GetCluster() { return m_cluster; }
	DirectX::XMVECTOR GetPosition() { return m_position; }
	void SetHealth(int hp) { m_health = hp; }
	bool CheckRender();
	void Uninitialise();
	CellPtr operator[](int index);


};


#endif // !CELL_H
