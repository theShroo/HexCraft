#include "Cluster.h"
#include "Map.h"






Cluster::Cluster(Hex location, Map* Owner)
{
	m_location = location;
	m_owner = Owner;
}

Cluster::~Cluster()
{
	std::unordered_map<Hex, Cell*, hash_Hex>::iterator terminator;
	for (terminator = m_cluster.begin(); terminator != m_cluster.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}

}

Cell* Cluster::GetCell(Hex cell) {
	if (m_cluster.count(cell) < 1) {
		Cell* newCell = new Cell(Hex::HexToVector(cell), this);
		m_cluster[cell] = newCell;
		float surface = (m_owner->simplexNoise()->fractal(5, float(cell.x) * 10.0 / 9.0f, float(cell.y) * 10.0 / 9.0f)) * 30;
		if (cell.w < surface) {
			if (cell.w < surface - 10) {
				newCell->SetType("Cobblestone", 0, 1);
				newCell->SetHealth(15);
			}
			else {
				m_cluster[cell]->SetType("Dirt", 0, 1);
			}
		}
	}
	return m_cluster[cell];
}

Map* Cluster::GetOwner()
{
	return m_owner;
}
