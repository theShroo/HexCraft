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

using namespace HexLogic;


Cluster::Cluster(Hex bLocation, Map* Owner, int clustersize)
{
	m_b_location = bLocation;
	m_owner = Owner;
	m_clustersize = clustersize;
}

// terminator gets complex, TODO implement storage of cluster data to a file before cluster is destroyed.
Cluster::~Cluster()
{
}

// safe cell fetcher. this method returns the target cell, creating one if none exist.
CellPtr Cluster::GetCell(Hex cell) {
	auto target = m_cluster.find(cell);
	if (target == m_cluster.end()) {
		CellPtr newCell( new Cell(HexToVector(cell), this));
		m_cluster[cell] = newCell;
		float surface = 0.0f; // (m_owner->simplexNoise()->fractal(5, float(cell.x) * 10.0f / 9.0f, float(cell.y) * 10.0f / 9.0f)) * 30;
		if (cell.w < surface) {
			if (cell.w < surface - 10) {
				newCell->SetType("Cobblestone", 0, 1);
				newCell->SetHealth(1);

			}
			else {
				
				
				// Debug code to divide up an area int discrete hexes to shiow cluster ownership.
				int count = (m_b_location.x - m_b_location.y) % 3;
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
				Hex s_location = newCell->GetLocation();
				Hex s_target = bigtosmall(m_b_location, m_clustersize);
				s_location.w = s_target.w;
				if (s_location == s_target) {
					newCell->SetType("Ammo", 0, 1);
				}

				// newCell->SetType("Dirt", 0, 1);
				// newCell->SetHealth(1);
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

// using smart pointers removes any need for this function


//// sterile cell fetcher, returning a nullptr if the cell does not exist.
//CellPtr Cluster::CheckCell(Hex cell)
//{
//	auto target = m_cluster.find(cell);
//	if (target == m_cluster.end()) {
//		return nullptr;
//	}
//	else {
//		return target->second;
//	}
//}

void Cluster::Update(float timestep, std::vector<GameObject*>& entitiesToUpdate, DirectX::XMVECTOR center)
{
	if (!m_initialised) {
		m_initialised = 1;
		_Initialise();
	}
	std::unordered_map<PointerKey, CellPtr, PointerHash>::iterator Update_iter;   // CELL iterator
	std::unordered_map<PointerKey, GameObject*, PointerHash>::iterator iter_A; // special iterator to iterate through an entity list

	for (Update_iter = m_cellUpdatables.begin(); Update_iter != m_cellUpdatables.end(); Update_iter++) {	// for all cells in the Cluster
		CellPtr currentCell = Update_iter->second;
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


void Cluster::DisableUpdate(CellPtr cell) {
	if (m_cellUpdatables.count(*cell) > 0) {
		m_cellUpdatables.erase(*cell);
	}
}

void Cluster::EnableUpdate(CellPtr cell) {
	if (m_cellUpdatables.count(*cell) == 0) {
		m_cellUpdatables[*cell] = cell;
	}
}

void Cluster::DisableRender(CellPtr cell) {
	if (m_cellRenderables.count(*cell) > 0) {
		m_cellRenderables.erase(*cell);
	}
}

void Cluster::EnableRender(CellPtr cell) {
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
	std::vector<CellPtr> cleaned;
	// if a cell has no entities, dont update it.
	for (auto i = m_cluster.begin(); i != m_cluster.end(); i++) {

		// at the moment there are no conditions that require a cell to be active.
		//if (i->second->GetEntities()->size() > 0) {
		//	EnableUpdate(i->second);
		//}
		//else {
		//	DisableUpdate(i->second);
		//}
		if (i->second->CheckRender()) {
			EnableRender(i->second);
		}
		else {
			DisableRender(i->second);
		}
	}
}

void Cluster::_Initialise() {
	Hex location = bigtosmall(m_b_location, m_clustersize);
	std::vector<Hex> plane;
	for (int i = 0; i <= m_clustersize; i++) {
		m_owner->GetRing(plane, location, i);
	}

	for (int i = 0; i < plane.size(); i++) {
		for (int j = 0; j < m_clustersize; j++) {
			Hex target = plane[i];
			target.w += j;
			GetCell(target);
		}
	}
	Clean(location);

}

// using smart pointers to avoid using this function at all

//void Cluster::_Deinitialise() {
//	Hex location = bigtosmall(m_b_location, m_clustersize);
//	std::vector<Hex> plane;
//	for (int i = 0; i <= m_clustersize+1; i++) {
//		m_owner->GetRing(plane, location, i);
//	}
//	Cell* target = nullptr;
//	for (int i = 0; i < plane.size(); i++) {
//		for (int j = -1; j < m_clustersize+1; j++) {
//			target = CheckCell(Hex{ 0,0,0, j } +plane[i]);
//			if (target) target->Uninitialise();
//		}
//	}
//}