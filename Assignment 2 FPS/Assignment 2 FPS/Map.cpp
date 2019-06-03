#include "Map.h"
#include "Loot.h"
#include "Bullets.h"
#include "AIPlayer.h"
#include "FPSPlayer.h"
#include "Cell.h"


using namespace DirectX;



Map::Map() {

	// initialise the direction translator.
	m_directions.push_back(Hex{ +1, -1, 0, 0 });
	m_directions.push_back(Hex{ +1, 0, -1, 0 });
	m_directions.push_back(Hex{ 0, +1, -1, 0 });
	m_directions.push_back(Hex{ -1, +1, 0, 0 });
	m_directions.push_back(Hex{ -1, 0, +1, 0 });
	m_directions.push_back(Hex{ 0, -1, +1, 0 });
	// initialise the floor
	m_generator = std::default_random_engine(unsigned(std::chrono::system_clock::now().time_since_epoch().count()));
	m_spawnchance = std::uniform_int_distribution<int> (1, 1000000);
	m_simplexNoise = new SimplexNoise(0.003f, 200.0f);
	// Initialise cluster logic.

}



// because we are storing references in the hash table we need to delete all the cells stored in the hash table manually, and we do this by iterating over the table.
// and deleting the "second", (this is the element of the dictionary entry that stores the 'object', the "first" element is the 'key'.)
Map::~Map() {
	//TODO Update this for Chunk logic
std::unordered_map<Hex, Cluster*, hash_Hex>::iterator terminator;
	for (terminator = m_map.begin(); terminator != m_map.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}
}


// Get Cell function updated in the map class, it now passes a loarge chunk of logic into the cluster class, 
// and breaks the map up into predefined 'chunks' to allow for smooth loading and unloading of cell groups

Cell* Map::GetCell(Hex cell) {
	// for the target cell we need to locate its cluster 
	Hex clusterLoc = Hex::smalltobig(cell, m_clusterSize);
	return GetCluster(clusterLoc)->GetCell(cell);
}

// cluster fetch function
Cluster* Map::GetCluster(Hex cluster)
{
	if (m_map.count(cluster) < 1) {
		m_map[cluster] = new Cluster(cluster, this,	m_clusterSize);
	}
	return m_map[cluster];
}

void Map::Initialise(Hex position, int distance)
{
	std::vector<Hex> zone;
	GetLocalMap(zone, Hex::smalltobig(position, m_clusterSize), distance);
	for (int i = 0; i < zone.size(); i++) {
		Cluster* cluster = GetCluster(zone[i]);
		m_ActiveClusters[*cluster] = cluster;
	}
}


// add an overload to accept coordinates.
Cell* Map::GetCell(int x, int y, int height) {
	return GetCell(Hex{ x,y,-x - y, height });
}


// function to identify an area around a target cell, primarily for rendering and updating.
void Map::GetLocalMap(std::vector<Hex> &localMap, Hex center, int range) {
	std::vector<Hex> results;
	for (int i = -range; i <= range; i++) {
		for (int j = max(-range, -i - range); j <= min(range, -i + range); j++) {
			for (int k = -range; k <= range; k++) {
				localMap.push_back(Hex{ i, j, -i - j, k }+center);
			}
		}
	}
}

void Map::GetLocalMap2D(std::vector<Hex> &localMap, Hex center, int range) {
	std::vector<Hex> results;
	for (int i = -range; i <= range; i++) {
		for (int j = max(-range, -i - range); j <= min(range, -i + range); j++) {
			localMap.push_back(Hex{ i, j, -i - j, 0 }+center);
		}
	}
}


// an update function optimised to only update entites within the local zone.
// this function also checks for collisions in an optimised area to reduce collision cost.
// to maintain this optimisation each cell keeps a list of entities contained within, and
// these lists must be updated AFTER all objects have been updated, AND the check has been 
// made for colisions. this update also checks for expired entities and removes them, and 
// performs any spawn functions that are queued.

void Map::Update(float timestep, XMVECTOR center) {
	std::vector<GameObject*> movedZone;																// list of objects that have left their zone.
	std::vector<GameObject*> entitiesToUpdate;

	std::unordered_map<PointerKey, GameObject*, PointerHash>::iterator iter_A, iter_B; // special iterator to iterate through an entity list
	std::unordered_map<PointerKey, Cluster*, PointerHash>::iterator Cluster_Update_iter;   // Cluster iterator 
	for (Cluster_Update_iter = m_ActiveClusters.begin(); Cluster_Update_iter != m_ActiveClusters.end(); Cluster_Update_iter++) {	// for all Clusters in the update area
		Cluster* currentCluster = Cluster_Update_iter->second;
		//	// Cells MUST be updated before any calls can be made to them, they have data that must be initialised before it can be used that is set on first update.
		currentCluster->Update(timestep, entitiesToUpdate, movedZone, center);

	}
	for (int i = 0; i < entitiesToUpdate.size(); i++) {
		// do colisions for interactive objects
		GameObject* entity = entitiesToUpdate[i];
		if (entity) {
			PhysicsObject* active = entity->GetInteractive();
			if (active) {
				Cell* cell = active->GetLocation();
				if (!cell->IsPassable()) {
					active->DoStanding(cell->GetLocation().w + 1.0f, this);
				}
				cell = cell->GetNeigbours()[9];  // look up cell by index in neighbours list, its MUCH faster than any other method. (index 9 is below)
				// new standing code, much more efficient than performing collisions on every hex in the zone.
				if (!cell->IsPassable()) {
					active->DoStanding(cell->GetLocation().w + 1.0f, this);
				}

				for (int i = 0; i < 20; i++) {
					std::unordered_map<PointerKey, GameObject*, PointerHash>* potentials = active->GetLocation()->GetNeigbours()[i]->GetEntities();
					if (potentials->size() > 0) {
						for (iter_B = potentials->begin(); iter_B != potentials->end(); iter_B++) {
							GameObject* potential = iter_B->second;
							PhysicsObject* interactive = potential->GetInteractive();			// if interactive then collide
							if (interactive) {
								if (interactive != active) {
									if (Collisions::CheckCollision(active->GetBounds(), interactive->GetBounds())) {
										// do normal colision here.
										active->DoCollision(interactive, this);
									}
								}
							}
						}
					}
				}
			}
		}

		if (entity) {
			Loot* loot = entity->GetLoot();							// if loot
			if (loot) {
				XMVECTOR target = center;
				target.m128_f32[1] += 1.0f;
				loot->ApplyForce(DirectX::XMVector3Normalize(target - loot->GetPosition()) * timestep * 0.05f);
				for (int i = 0; i < 20; i++) {
					std::unordered_map<PointerKey, GameObject*, PointerHash>* potentials = loot->GetLocation()->GetNeigbours()[i]->GetEntities();
					if (potentials->size() > 0) {
						for (iter_B = potentials->begin(); iter_B != potentials->end(); iter_B++) {
							GameObject* potential = iter_B->second;
							Player* player = potential->GetPlayer();			// hits an active object
							if (player) {
								if (Collisions::CheckCollision(player->GetBounds(), loot->GetBounds())) {
									loot->DoCollision(player, this);			// get the loot
									entity = 0;									// disable further interaction with the entity as it is scheduled for removal
								}
							}
						}
					}
				}
			}
		}
		// test if this is a bullet.
		if (entity) {
			Bullets* bullet = entity->GetBullet();
			if (bullet) {
				Cell* location = bullet->GetLocation();
				if (!location->IsPassable()) {
					bullet->DoCollision(location, this);		// bullets remove themselves upon coillision.
					entity = 0;
				}
				else {

					bool ingun = false;

					for (int i = 0; entity && i < 20; i++) {
						std::unordered_map<PointerKey, GameObject*, PointerHash>* potentials = entity->GetLocation()->GetNeigbours()[i]->GetEntities();
						if (potentials->size() > 0) {
							for (iter_B = potentials->begin(); iter_B != potentials->end() && entity; iter_B++) {
								GameObject* potential = iter_B->second;
								PhysicsObject* interactive = potential->GetInteractive();
								if (interactive) {
									if (Collisions::CheckCollision(bullet->GetBounds(), interactive->GetBounds())) {
										if (bullet->m_leftGun) {						// make sure the bullet has actually left the gun it was fired from.
											bullet->DoCollision(interactive, this);		// bullets remove themselves upon coillision.
											entity = 0;
											// impact code isnt working yet;
											// new Impact(GetXMVECTOR3(hit), "Unlit Texture Shader", "Bullet", "Impact");	// we also have the hit Hex for placing an impact animation.
										}
										else if (!ingun) {
											ingun = true;
										}
									}
								}
							}
						}
					}
					if (entity) {
						if (bullet->m_ttl < 0) {								// if the bullet has expired and it hasnt hit anything.
							this->RemoveObject(bullet);
							entity = 0;
						}
						else if (!ingun) {
							bullet->m_leftGun = true;
						}
					}
				}
			}
		}
	}
	// if it still needs to be updated push it back into a vector to be updated. this stops objects moving before other things can collide with them.
// then update all entites that remain to accept an update.
	for (int i = 0; i < entitiesToUpdate.size(); i++) {
		GameObject* entity = entitiesToUpdate[i];
		entity->Update(timestep);															// update them.
		Hex newHex = Hex::VectorToHex(entity->GetPosition());
		if (newHex != entity->GetLocation()->GetLocation()) {
			if (entity->GetPlayer()) {
				Cell* cell = GetCell(newHex);
				if (cell->IsPassable() && cell->GetNeigbours()[10]->IsPassable()) {  // cell->neighbours()[10] is the Cell above cell.
					movedZone.push_back(entity);
				}
				else if (cell->GetNeigbours()[10]->IsPassable() && cell->GetNeigbours()[10]->GetNeigbours()[10]->IsPassable()) { // nest neighbours 10 to get the cell two above the current. (still faster than hash table lookup)
					entity->Move(MathsHelper::GetXMVECTOR3(0, 1, 0));
					movedZone.push_back(entity);
				}
				else {
					XMVECTOR Hex = entity->GetLocation()->GetPosition();
					entity->setPosition(Hex + XMVector3Normalize(entity->GetPosition() - Hex)* 0.65f);
				}
			}
			else {
				movedZone.push_back(entity);													// flag them for cell reHex.
			}
		}
	}

	// move entites location flags
	for (unsigned i = 0; i < movedZone.size(); i++) {												// move all flagged objects to their new cells
		GameObject* entity = movedZone[i];
		Cell* cell =entity->GetLocation();
		cell->GetEntities()->erase(entity->operator PointerKey());	// remove the object from its old cell.
		if (cell->GetEntities()->size() == 0) {						// if this cell has no entities left then remove it from the update list
			cell->DisableUpdate();
			m_renderCheckQueue.push_back(cell);						// and retest for renderability
		}
		Hex hex = Hex::VectorToHex(entity->GetPosition());
		cell = GetCell(hex);
		cell->GetEntities()->insert({ entity->operator PointerKey(), entity });		// add the object to its new cell.
		entity->SetLocation(cell);													// update the objects location.							
		cell->EnableUpdate();
		m_renderCheckQueue.push_back(cell);
		// add the objects new cell to the updateables list.
	}		
	// add new items to the map
	for (unsigned i = 0; i < m_additionQueue.size(); i++) {											// add all items queued for inclusion.

		Hex hex = m_additionQueue[i].second;
		Cell* cell = GetCell(hex);
		std::unordered_map<PointerKey, GameObject*, PointerHash>* entitylist = cell->GetEntities();
		entitylist->operator[](m_additionQueue[i].first->operator PointerKey()) = m_additionQueue[i].first;		// add the object to its new cell.
		cell->EnableUpdate();																				// to prevent errors we will update teh cell when it is added to the update list.
		cell->Update(0); 
		m_additionQueue[i].first->SetLocation(cell);
	}
	m_additionQueue.clear();
	// remove deleted items from the map (basic garbage collection)
	for (unsigned i = 0; i < m_removalQueue.size(); i++) {
		Hex hex = m_removalQueue[i].second;
		Cell* cell= GetCell(hex);
		std::unordered_map<PointerKey, GameObject*, PointerHash>* entitylist = cell->GetEntities();
		entitylist->erase(m_removalQueue[i].first->operator PointerKey());
		delete m_removalQueue[i].first;
		if (cell->GetEntities()->size() < 1) {
			cell->DisableUpdate();
		}

	}
	m_removalQueue.clear();


}

// the map is an object container holding renderable objects, so we need a special iterator render only the correct items.
void Map::RenderLocal(XMVECTOR location, Direct3D* renderer, Camera* cam) {
	// check changed cells for rendering
	Hex here = Hex::VectorToHex(location);
	for (int i = 0; i < m_renderCheckQueue.size(); i++) {
		Cell* cell = m_renderCheckQueue[i];
		if (cell) {
			if (cell->CheckRender()) {
				cell->EnableRender();
			}
			else {
				cell->DisableRender();
			}
		}
	}
	m_renderCheckQueue.clear();
	for (auto i = m_ActiveClusters.begin(); i != m_ActiveClusters.end(); i++) {
		i->second->Render(renderer, cam);
	}
}

SimplexNoise* Map::simplexNoise()
{
	return m_simplexNoise;
}



// direction translator for iterative functions.
Hex Map::GetDirection(int direction) {
	return m_directions[direction];
}

int Map::GetDirection(Hex direction) {
	if (direction.x == 1) {
		if (direction.y == -1) {
			return 0;
		}
		else {
			return 1;

		}
	}
	else if (direction.x == -1) {
		if (direction.y == 1) {
			return 3;
		}
		else {
			return 4;
		}	
	}
	else {
		if (direction.y == 1) {
			return 2;
		}
		else if(direction.y == -1){
			return 5;
		}
	}
	return -1;
}

// method to get a ring of hexes
void Map::GetRing(std::vector<Hex> &HexList, Hex center, int radius) {

	if (radius == 0) {
		HexList.push_back(center);
	}
	else {
		Hex cube = center + m_directions[4] * radius;
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < radius; j++) {
				HexList.push_back(cube);
				cube = cube + GetDirection(i);
			}
		}
	}
}

// method to get all hexes in a line.
void Map::GetLine(std::vector<Hex> &HexList, Hex a, Hex b) {
	int samples = a.distance(b) + 1;
	for (int i = 0; i < samples; i++) {
		HexList.push_back(Hex::Float4ToHex(Hex::LerpHex(a, b, (1.0f / samples)*i)));
	}
}

void Map::AddObject(GameObject* object) {
	m_additionQueue.push_back({ object, Hex::VectorToHex(object->GetPosition()) });
}

void  Map::RemoveObject(GameObject* object) {
	m_removalQueue.push_back({ object, Hex::VectorToHex(object->GetPosition()) });															// remove the object from its cell.
}

// method for pathing ai to a given location, provides a list of accessable hexes from a given start point. not yet implemented


GameObject* Map::AddAI(int type, Player* target, float range) {

	std::uniform_real_distribution<float> location(-30.0f, 30.0f);
	AIPlayer* ai = new AIPlayer(MathsHelper::GetXMVECTOR3(location(m_generator), 0.0f, location(m_generator)), "Diffuse Texture Fog Shader", "Enemy", "Player Light Red", type);
	ai->SetTarget(target);
	AddObject(ai);
	return ai;
}

GameObject* Map::AddLoot(std::string name, std::string shader, std::string mesh, std::string texture, DirectX::XMVECTOR location, int qty) {
	GameObject* loot = new Loot(shader, mesh, texture, location, name, qty);
	AddObject(loot);
	return loot;
}
// add a cells neigbours to a list to be checked next render cycle.
void Map::RenderCheck(Cell* cell) {
	for (int i = 0; i < 20; i++) {
		m_renderCheckQueue.push_back(cell->GetNeigbours()[i]);
	}
}


void Map::CleanZones(Hex center) {
	// if a cell has no entities, dont update it.
	for (auto i = m_ActiveClusters.begin(); i != m_ActiveClusters.end(); i++) {
		i->second->Clean(center);
	}
}


void Map::UpdateZones(Hex center, Hex h_direction, int updateDistance) {
	// check if we are moving up or down, this is important as the second condition only calculates for lateral traversal
	if (h_direction.w != 0) {
		std::vector<Hex> plane;
		Cluster* cluster;
		GetLocalMap2D(plane, center + (Hex{0,0,0,h_direction.w}* updateDistance), updateDistance);
		for (int i = 0; i < plane.size(); i++) {
			cluster = GetCluster(plane[i]);
			m_ActiveClusters[*cluster] = cluster;
		}
		plane.clear();
		h_direction.w = 0;				// if h_direction is also a planar direction we need to test for that.
	}
	int i_direction = GetDirection(h_direction);
	if (i_direction >= 0) {
		// test for a valid direction before we do anything here, otherwise we risk weird/untracable crashes/bugs.
		// get all the directions and units that we need to calculate the update and render check planes;
		int i_behind = (i_direction + 3) % 6;
		int i_leadingdirection1 = (i_direction + 2) % 6, i_leadingdirection2 = (i_direction + 4) % 6;

		Hex h_behind = GetDirection(i_behind);
		Hex h_leadingdirection1 = GetDirection(i_leadingdirection1);
		Hex h_leadingdirection2 = GetDirection(i_leadingdirection2);
		// do updateables and render checks at the same time for the leading planes, check for both update requirement and render requirement.

		// leading planes * 2
		Cluster* cluster;
		Hex leading = center + (h_direction * updateDistance);
		for (int i = -updateDistance; i <= updateDistance; i++) {
			Hex currentl = Hex{ 0,0,0,i } +leading;
			for (int j = 0; j <= updateDistance; j++) {
				cluster = GetCluster(currentl + h_leadingdirection1 * j);		// leading plane 1
				m_ActiveClusters[*cluster] = cluster;
				cluster = GetCluster(currentl + h_leadingdirection2 * j);		// leading plane 2
			}
		}
	}
	// clear out Active clusters that are out of range
	std::vector<Cluster*> outsideDistance;
	//check the distance from the center to each updateable to determine if its out of range (this is faster than checking an entire plane)
	for (auto i = m_ActiveClusters.begin(); i != m_ActiveClusters.end(); i++) {
		if (i->second->GetLocation().distance(center) > updateDistance) {
			outsideDistance.push_back(i->second);
		}
		else if (i->second->GetLocation().w > updateDistance) {
			outsideDistance.push_back(i->second);
		}
	}
	for (unsigned int i = 0; i < outsideDistance.size(); i++) {
		m_ActiveClusters.erase(*outsideDistance[i]);
	}
	outsideDistance.clear();
}


bool Map::IncrementZone(Hex center, int updateDistance) {
	std::vector<Hex> ring;
	std::vector<Hex> plane;
	Cluster* cluster;
	for (int i = 0; i < updateDistance; i++) {
		GetRing(plane, center, i);
	}
	GetRing(ring, center, updateDistance);
	bool incremented = false;
	for (int i = 0; i < ring.size() && incremented == false; i++) {
		for (int j = -updateDistance; j <= updateDistance && incremented == false; j++) {
			cluster = GetCluster(Hex{ 0,0,0,j } +ring[i]);
			if (m_ActiveClusters.count(*cluster) == 0) {

				m_ActiveClusters[*cluster] = cluster;
				incremented = true;
			}
		}
	}

	for (int i = 0; i < plane.size() && incremented == false; i++) {
		cluster = GetCluster(Hex{ 0,0,0, updateDistance } +plane[i]);
		if (m_ActiveClusters.count(*cluster) == 0) {
			m_ActiveClusters[*cluster] = cluster;
			incremented = true;
		}
		cluster = GetCluster(Hex{ 0,0,0, -updateDistance } +plane[i]);
		if (m_ActiveClusters.count(*cluster) == 0) {
			m_ActiveClusters[*cluster] = cluster;
			incremented = true;
		}
	}	
	return incremented;

}