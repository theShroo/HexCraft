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
	}
	return m_cluster[cell];
}

Map* Cluster::GetOwner()
{
	return m_owner;
}

void Cluster::Update(float timestep, std::vector<GameObject*>& movedZone, std::vector<GameObject*>& entitiesToUpdate, XMVECTOR center)
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
		std::unordered_map<PointerKey, GameObject*, PointerHash>* entities = currentCell->GetEntities();		// get all game objects.
		if (entities->size() > 0) {
			for (iter_A = entities->begin(); iter_A != entities->end(); iter_A++) {
				GameObject* entity = iter_A->second;
				if (entity) {
					entitiesToUpdate.push_back(entity);
				}
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
}


void Cluster::Clean(Hex center) {
	std::vector<Cell*> cleaned;
	// if a cell has no entities, dont update it.
	for (auto i = m_cluster.begin(); i != m_cluster.end(); i++) {
		if (i->second->GetEntities()->size() > 0) {
			EnableUpdate(i->second);
		}
		else {
			DisableUpdate(i->second);
		}
		if (i->second->CheckRender()) {
			EnableRender(i->second);
		}
		else {
			DisableRender(i->second);
		};
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
			GetCell(Hex{ 0,0,0, j } +plane[i])->EnableRender();
		}
	}
	Clean(location);

}