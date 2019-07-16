/*
	Cluster.h written by Russell Harland 2019

	a class to describe the cluster class
	contains all the methods used to initialise and interface with a specific block of cells. 


*/



#include "Cluster.h"
#include "Map.h"
#include "Loot.h"
#include "Bullets.h"
#include "AIPlayer.h"
#include "FPSPlayer.h"
#include "Cell.h"


Cluster::Cluster(Hex location, Map* Owner, int clustersize)
{
	m_location = location;
	m_owner = Owner;
	m_clustersize = clustersize;
}

// terminator gets complex, TODO implement storage of cluster data to a file before cluster is destroyed.
Cluster::~Cluster()
{
	// delete all the cells in the cluster
	for (auto terminator = m_cluster.begin(); terminator != m_cluster.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}
	m_cluster.clear();

	// delete all the entities in the cluster
	for (auto terminator = m_entities.begin(); terminator != m_entities.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}
	m_entities.clear();
	// final method to uninitialise all cells whose pointers have been invalidated by the deletion of the cluster.
	_Deinitialise();



}

// safe cell fetcher. this method returns the target cell, creating one if none exist.
Cell* Cluster::GetCell(Hex cell) {
	auto target = m_cluster.find(cell);
	if (target == m_cluster.end()) {
		Cell* newCell = new Cell(Hex::HexToVector(cell), this);
		m_cluster[cell] = newCell;
		float surface = (m_owner->simplexNoise()->fractal(5, float(cell.x) * 10.0f / 9.0f, float(cell.y) * 10.0f / 9.0f)) * 30;
		if (cell.w < surface) {
			if (cell.w < surface - 10) {
				newCell->SetType("Cobblestone", 0, 1);
				newCell->SetHealth(1);

			}
			else {
				
				/*
				// Debug code to divide up an area int discrete hexes to shiow cluster ownership.
				int count = (m_location.x - m_location.y) % 3;
				if (count < 0) count += 3;
				switch (count) {
				case 0:
					newCell->SetType("Dirt", 0, 1);
					newCell->SetHealth(15);
					break;
				case 1:
					newCell->SetType("Cobblestone", 0, 1);
					newCell->SetHealth(15);
					break;
				case 2:
					newCell->SetType("Wall", 0, 1);
					newCell->SetHealth(15);
					break;
				default:
					newCell->SetType("Ammo", 0, 1);
					newCell->SetHealth(15);
					break;
				}
				*/
				newCell->SetType("Dirt", 0, 1);
				newCell->SetHealth(1);
			}
		}
		return newCell;
	}
	return target->second;
}

Map* Cluster::GetOwner()
{
	return m_owner;
}

int Cluster::GetCount() {
	return int(m_cluster.size());
}
// sterile cell fetcher, returning a nullptr if the cell does not exist.
Cell* Cluster::CheckCell(Hex cell)
{
	auto target = m_cluster.find(cell);
	if (target == m_cluster.end()) {
		return nullptr;
	}
	else {
		return target->second;
	}
}

void Cluster::Update(float timestep, std::vector<GameObject*>& entitiesToUpdate, XMVECTOR center)
{
	if (!m_initialised) {
		m_initialised = 1;
		_Initialise();
	}
	std::unordered_map<PointerKey, Cell*, PointerHash>::iterator Update_iter;   // CELL iterator
	std::unordered_map<PointerKey, GameObject*, PointerHash>::iterator iter_A; // special iterator to iterate through an entity list

	for (Update_iter = m_cellUpdatables.begin(); Update_iter != m_cellUpdatables.end(); Update_iter++) {	// for all cells in the Cluster
		Cell* currentCell = Update_iter->second;
		// Cells MUST be updated before any calls can be made to them, they have data that must be initialised before it can be used that is set on first update.
		currentCell->Update(timestep);
	}
	if (m_entities.size() > 0) {
		for (iter_A = m_entities.begin(); iter_A != m_entities.end(); iter_A++) {
			GameObject* entity = iter_A->second;
			if (entity) {
				entitiesToUpdate.push_back(entity);
			}
		}
	}
}


void Cluster::DisableUpdate(Cell* cell) {
	if (m_cellUpdatables.count(*cell) > 0) {
		m_cellUpdatables.erase(*cell);
	}
}

void Cluster::EnableUpdate(Cell* cell) {
	if (m_cellUpdatables.count(*cell) == 0) {
		m_cellUpdatables[*cell] = cell;
	}
}

void Cluster::DisableRender(Cell* cell) {
	if (m_cellRenderables.count(*cell) > 0) {
		m_cellRenderables.erase(*cell);
	}
}

void Cluster::EnableRender(Cell* cell) {
	if (m_cellRenderables.count(*cell) == 0) {
		m_cellRenderables[*cell] = cell;
	}
}


void Cluster::Render(Direct3D* renderer, Camera* cam) {
	for (auto i = m_cellRenderables.begin(); i != m_cellRenderables.end(); i++) {
		i->second->Render(renderer, cam);
	}
	if (m_entities.size() > 0) {
		for (auto m_iter = m_entities.begin(); m_iter != m_entities.end(); m_iter++) {
			m_iter->second->Render(renderer, cam);
		}
	}
}

std::unordered_map<PointerKey, GameObject*, PointerHash>* Cluster::GetEntities() {
	return &m_entities;
}

void Cluster::Clean(Hex center) {
	std::vector<Cell*> cleaned;
	// if a cell has no entities, dont update it.
	for (auto i = m_cluster.begin(); i != m_cluster.end(); i++) {

		// at the moment there are no conditions that require a cell to be active.
		//if (i->second->GetEntities()->size() > 0) {
		//	EnableUpdate(i->second);
		//}
		//else {
		//	DisableUpdate(i->second);
		//}
		i->second->CheckRender();
	}
}

void Cluster::_Initialise() {
	Hex location = Hex::bigtosmall(m_location, m_clustersize);
	std::vector<Hex> plane;
	for (int i = 0; i <= m_clustersize; i++) {
		m_owner->GetRing(plane, location, i);
	}

	for (int i = 0; i < plane.size(); i++) {
		for (int j = 0; j < m_clustersize; j++) {
			GetCell(Hex{ 0,0,0, j } +plane[i]);
		}
	}
	Clean(location);

}

void Cluster::_Deinitialise() {
	Hex location = Hex::bigtosmall(m_location, m_clustersize);
	std::vector<Hex> plane;
	for (int i = 0; i <= m_clustersize+1; i++) {
		m_owner->GetRing(plane, location, i);
	}
	Cell* target = nullptr;
	for (int i = 0; i < plane.size(); i++) {
		for (int j = -1; j < m_clustersize+1; j++) {
			target = CheckCell(Hex{ 0,0,0, j } +plane[i]);
			if (target) target->Uninitialise();
		}
	}
}