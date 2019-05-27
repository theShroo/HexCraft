#include "Map.h"
#include "Loot.h"
#include "Bullets.h"
#include "AIPlayer.h"
#include "FPSPlayer.h"

using namespace DirectX;

XMVECTOR Map::HexToVector(Hex coordinates) {
	return MathsHelper::GetXMVECTOR3(1.732f*(coordinates.x + (float)coordinates.y /2), coordinates.w, (float)3/2 * coordinates.y);
}

// this function takes Hex data as an xmfloat4 and rounds it to the nearest hex.
Hex Map::Float4ToHex(XMFLOAT4 coordinates) {

	float rx = round(coordinates.x);
	float ry = round(coordinates.y);
	float rz = round(coordinates.z);

	float x_diff = abs(rx - coordinates.x);
	float y_diff = abs(ry - coordinates.y);
	float z_diff = abs(rz - coordinates.z);

	if (x_diff > y_diff && x_diff > z_diff) {
		rx = -ry - rz;
	}
	else if (y_diff > z_diff) {
		ry = -rx - rz;
	}
	else {
		rz = -rx - ry;
	}
	return Hex{ int(rx), int(ry), int(rz), int(round(coordinates.w))};
}

Hex Map::VectorToHex(XMVECTOR coordinates) {
	// this conversion code gets a little messy, but without it the getlocal function (the most noticable of the functions that use this conversion)
	// would all generate a poisitional bias as the distcance from 0 increases
	// this function also converts the XMVECTOR coordinates to hex format, but leaves the floating point data intact for the xmfloat to hex function to round correctly


	XMFLOAT4 coords;
	XMStoreFloat4(&coords, coordinates);
	coords.x = (coords.x * 1.732f / 3) - (coords.z / 3);
	coords.z /= 1.5f;
	return Float4ToHex(XMFLOAT4(coords.x, coords.z, -coords.x - coords.z, coords.y));
}

XMFLOAT4 Map::LerpHex(Hex a, Hex b, float t){
	XMFLOAT4 result(MathsHelper::LerpFloat(a.x, b.x, t),
		MathsHelper::LerpFloat(a.y, b.y, t),
		MathsHelper::LerpFloat(a.z, b.z, t),
		MathsHelper::LerpFloat(a.w, b.w, t));
	return result;
}


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
}


// because we are storing references in the hash table we need to delete all the cells stored in the hash table manually, and we do this by iterating over the table.
// and deleting the "second", (this is the element of the dictionary entry that stores the 'object', the "first" element is the 'key'.)
Map::~Map() {
	std::unordered_map<Hex, Cell*, hash_Hex>::iterator terminator;
	for (terminator = m_map.begin(); terminator != m_map.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}
}


// the get cell function, which checks for the cell, and makes a new cell if none exists.
// the default cell is unpassable, and contains no items, this allows the map to correctly return cells outside the predefined map area.
Cell* Map::GetCell(Hex cell) {
	if (m_map.count(cell) < 1) {
		Cell* newCell = new Cell(HexToVector(cell), this);
		m_map[cell] = newCell;
		float surface = (m_simplexNoise->fractal(5, float(cell.x)* 10.0 / 9.0f, float(cell.y)* 10.0 / 9.0f)) * 30;
		if (cell.w < surface) {
			if (cell.w < surface - 10) {
				newCell->SetType("Cobblestone", 0, 1);
				newCell->SetHealth(15);
			}
			else {
				m_map[cell]->SetType("Dirt", 0, 1);
			}
		}
	}
	return m_map[cell];
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

void Map::Update(float timestep, XMVECTOR center, std::unordered_map < PointerKey, Cell*, PointerHash> &updatables) {
	std::vector<GameObject*> movedZone;																// list of objects that have left their zone.
	std::vector<GameObject*> entitiesToUpdate;

	std::unordered_map<PointerKey, GameObject*, PointerHash>::iterator iter_A, iter_B; // special iterator to iterate through an entity list
	std::unordered_map<PointerKey, Cell*, PointerHash>::iterator Update_iter;
	for (Update_iter = updatables.begin(); Update_iter != updatables.end(); Update_iter++) {	// for all cells in the update area
		Cell* currentCell = Update_iter->second;
		// Cells MUST be updated before any calls can be made to them, they have data that must be initialised before it can be used that is set on first update.
		currentCell->Update(timestep);
		std::unordered_map<PointerKey, GameObject*, PointerHash> *entities = currentCell->GetEntities();		// get all game objects.
		if (entities->size() > 0) {
			for (iter_A = entities->begin(); iter_A != entities->end(); iter_A++) {
				// lets optimise this code by running the update checks here for collisions and whatnot, then after all colisions have been done update the 
				// entities based on the results of this part of the update loop by pushing them back into a vector of objects to be updated.
				GameObject* entity = iter_A->second;
				Cell** neigbours = currentCell->GetNeigbours(); // we need all the neigbour cells for this part of the update cycle, so we store a static list of neigbours
																// in the cell to reduce lookup time.
				
				// do colisions for interactive objects
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
							std::unordered_map<PointerKey, GameObject*, PointerHash>* potentials = neigbours[i]->GetEntities();
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
						loot->ApplyForce(DirectX::XMVector3Normalize(target - loot->GetPosition())* timestep* 0.05f);
						for (int i = 0; i < 20; i++) {
							std::unordered_map<PointerKey, GameObject*, PointerHash>* potentials = neigbours[i]->GetEntities();
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
								std::unordered_map<PointerKey, GameObject*, PointerHash>* potentials = neigbours[i]->GetEntities();
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
									RemoveObject(bullet);
									entity = 0;
								}
								else if (!ingun) {
									bullet->m_leftGun = true;
								}
							}
						}
					}
				}
				// if it still needs to be updated push it back into a vector to be updated. this stops objects moving before other things can collide with them.
				if (entity) {
					entitiesToUpdate.push_back(iter_A->second);
				}
			}
		}
	}
// then update all entites that remain to accept an update.
	for (int i = 0; i < entitiesToUpdate.size(); i++) {
		GameObject* entity = entitiesToUpdate[i];
		entity->Update(timestep);															// update them.
		Hex newHex = VectorToHex(entity->GetPosition());
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
			updatables.erase(*cell);
			m_renderCheckQueue.push_back(cell);						// and retest for renderability
		}
		Hex hex = VectorToHex(entity->GetPosition());
		cell = GetCell(hex);
		cell->GetEntities()->insert({ entity->operator PointerKey(), entity });		// add the object to its new cell.
		entity->SetLocation(cell);													// update the objects location.							
		updatables[*cell] = cell;
		m_renderCheckQueue.push_back(cell);
		// add the objects new cell to the updateables list.
	}		
	// add new items to the map
	for (unsigned i = 0; i < m_additionQueue.size(); i++) {											// add all items queued for inclusion.

		Hex hex = m_additionQueue[i].second;
		Cell* cell = GetCell(hex);
		std::unordered_map<PointerKey, GameObject*, PointerHash>* entitylist = cell->GetEntities();
		entitylist->operator[](m_additionQueue[i].first->operator PointerKey()) = m_additionQueue[i].first;		// add the object to its new cell.
		updatables[*cell] = cell;																				// to prevent errors we will update teh cell when it is added to the update list.
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
		if (cell->GetEntities()->size() == 0) {
			updatables.erase(*cell);
		}

	}
	m_removalQueue.clear();


}

// the map is an object container holding renderable objects, so we need a special iterator render only the correct items.
void Map::RenderLocal(XMVECTOR location, Direct3D* renderer, Camera* cam, std::unordered_map < PointerKey, Cell*, PointerHash> &renderables) {
	// check changed cells for rendering
	Hex here = VectorToHex(location);
	for (int i = 0; i < m_renderCheckQueue.size(); i++) {
		Cell* cell = m_renderCheckQueue[i];
		if (cell) {
			if (CheckRender(cell)) {
				renderables[*cell] = cell;
			}
			else {
				if (renderables.count(*cell) > 0) {
					renderables.erase(*cell);
				}
			}
		}
	}
	m_renderCheckQueue.clear();
	for (auto i = renderables.begin(); i != renderables.end(); i++) {
		i->second->Render(renderer, cam);
	}
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
		HexList.push_back(Float4ToHex(LerpHex(a, b, (1.0f / samples)*i)));
	}
}

void Map::AddObject(GameObject* object) {
	m_additionQueue.push_back({ object, VectorToHex(object->GetPosition()) });
}

void  Map::RemoveObject(GameObject* object) {
	m_removalQueue.push_back({ object, VectorToHex(object->GetPosition()) });															// remove the object from its cell.
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

void Map::CleanZones(std::unordered_map < PointerKey, Cell*, PointerHash> &Updateables, std::unordered_map < PointerKey, Cell*, PointerHash> &renderables, Hex center) {
	std::vector<Cell*> cleaned;
	// if a cell has no entities, dont update it.
	for (auto i = Updateables.begin(); i != Updateables.end(); i++) {
		if (i->second->GetEntities()->size() == 0) {
			cleaned.push_back(i->second);
		}
	}
	// perform the removal after the iterator has finished to prevent invalidating the iterator.
	for (int i = 0; i < cleaned.size(); i++) {
		Updateables.erase(*cleaned[i]);
	}
	cleaned.clear();
	// if a cell has no visible faces, dont render it.
	for (auto i = renderables.begin(); i != renderables.end(); i++) {
		if (!CheckRender(i->second)) {
			cleaned.push_back(i->second);
		}
	}

	for (int i = 0; i < cleaned.size(); i++) {
		renderables.erase(*cleaned[i]);
	}

}


bool Map::CheckRender(Cell* cell) {
	bool renderable = false;
	if (cell->GetEntities()->size() > 0) {
		renderable = true;
	}
	// this brute force render check replaces the math heavy version that only checks the cells on the visible faces,
	// the result is that cells that have no faces visible to the player are still rendered, BUT this is still twice as efficient compared to the other version.
	if (!renderable&& cell->IsSolid()) {
		for (int i = 0; i < 20 && !renderable; i++) {
			if (!cell->GetNeigbours()[i]->IsSolid()) {
				renderable = true;
			}
		}
	}
	return renderable;
}

void Map::UpdateZones(std::unordered_map < PointerKey, Cell*, PointerHash> &Updateables, std::unordered_map < PointerKey, Cell*, PointerHash> &renderables, Hex center, Hex h_direction, int updateDistance) {
	// check if we are moving up or down, this is important as the second condition only calculates for lateral traversal
	if (h_direction.w != 0) {
		std::vector<Hex> plane;
		Cell* cell;
		GetLocalMap2D(plane, center + (Hex{0,0,0,h_direction.w}* updateDistance), updateDistance);
		for (int i = 0; i < plane.size(); i++) {
			cell = GetCell(plane[i]);
			if (cell->GetEntities()->size() > 0) {
				Updateables[*cell] = cell;
			}
			if (CheckRender(cell)) {
				renderables[*cell] = cell;
			}
		}
		plane.clear();
		h_direction.w = 0;				// if h_direction is also a direction we need to test for that.
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
		Cell* cell;
		Hex leading = center + (h_direction * updateDistance);
		for (int i = -updateDistance; i <= updateDistance; i++) {
			Hex currentl = Hex{ 0,0,0,i } +leading;
			for (int j = 0; j <= updateDistance; j++) {
				cell = GetCell(currentl + h_leadingdirection1 * j);		// leading plane 1
				if (cell->GetEntities()->size() > 0) {
					Updateables[*cell] = cell;
				}
				if (CheckRender(cell)) {
					renderables[*cell] = cell;
				}
				cell = GetCell(currentl + h_leadingdirection2 * j);		// leading plane 2
				if (cell->GetEntities()->size() > 0) {
					Updateables[*cell] = cell;
				}
				if (CheckRender(cell)) {
					renderables[*cell] = cell;
				}
			}
		}
	}
	// clear out updateables that are out of range
	std::vector<Cell*> outsideDistance;
	//check the distance from the center to each updateable to determine if its out of range (this is faster than checking an entire plane)
	for (auto i = Updateables.begin(); i != Updateables.end(); i++) {
		if (i->second->GetLocation().distance(center) > updateDistance) {
			outsideDistance.push_back(i->second);
		}
		else if (i->second->GetLocation().w > updateDistance) {
			outsideDistance.push_back(i->second);
		}
	}
	for (unsigned int i = 0; i < outsideDistance.size(); i++) {
		Updateables.erase(*outsideDistance[i]);
	}
	outsideDistance.clear();

	// clear out renderables that have fallen out of range.
	for (auto i = renderables.begin(); i != renderables.end(); i++) {
		if (i->second->GetLocation().distance(center) > updateDistance + 5) {
			outsideDistance.push_back(i->second);
		}
		else if (i->second->GetLocation().w > updateDistance + 5) {
			outsideDistance.push_back(i->second);
		}
	}
	for (unsigned int i = 0; i < outsideDistance.size(); i++) {
		renderables.erase(*outsideDistance[i]);
	}
	outsideDistance.clear();
}


void Map::IncrementZone(std::unordered_map < PointerKey, Cell*, PointerHash> &Updateables, std::unordered_map < PointerKey, Cell*, PointerHash> &renderables, Hex center, int updateDistance) {
	std::vector<Hex> ring;
	std::vector<Hex> plane;
	Cell* cell;
	for (int i = 0; i < updateDistance; i++) {
		GetRing(plane, center, i);
	}
	GetRing(ring, center, updateDistance);
	for (int i = 0; i < ring.size(); i++) {
		for (int j = -updateDistance; j <= updateDistance; j++) {
			cell = GetCell(Hex{ 0,0,0,j } +ring[i]);
			if (cell->GetEntities()->size() > 0) {
				Updateables[*cell] = cell;
			}
			if (CheckRender(cell)) {
				renderables[*cell] = cell;
			}
		}
	}

	for (int i = 0; i < plane.size(); i++) {
		cell = GetCell(Hex{ 0,0,0, updateDistance} +plane[i]);
		if (cell->GetEntities()->size() > 0) {
			Updateables[*cell] = cell;
		}
		if (CheckRender(cell)) {
			renderables[*cell] = cell;
		}
		cell = GetCell(Hex{ 0,0,0, -updateDistance } +plane[i]);
		if (cell->GetEntities()->size() > 0) {
			Updateables[*cell] = cell;
		}
		if (CheckRender(cell)) {
			renderables[*cell] = cell;
		}
	}
}