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

void Cluster::Update(float timestep, std::vector<GameObject*>& movedZone, std::vector<GameObject*>& entitiesToUpdate)
{
	std::unordered_map<PointerKey, Cell*, PointerHash>::iterator Update_iter;   // CELL iterator
	std::unordered_map<PointerKey, GameObject*, PointerHash>::iterator iter_A, iter_B; // special iterator to iterate through an entity list

	for (Update_iter = m_cluster.begin(); Update_iter != m_cellUpdatables.end(); Update_iter++) {	// for all cells in the Cluster
		Cell* currentCell = Update_iter->second;
		// Cells MUST be updated before any calls can be made to them, they have data that must be initialised before it can be used that is set on first update.
		currentCell->Update(timestep);
		std::unordered_map<PointerKey, GameObject*, PointerHash>* entities = currentCell->GetEntities();		// get all game objects.
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
						loot->ApplyForce(DirectX::XMVector3Normalize(target - loot->GetPosition()) * timestep * 0.05f);
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
									m_owner->RemoveObject(bullet);
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
}


void Cluster::DisableUpdate(Cell* cell) {
	if (m_cellUpdatables.count(cell) > 0) {
		m_cellUpdatables.erase(*cell);
	}
}

void Cluster::EnableUpdate(Cell* cell) {
	if (m_cellUpdatables.count(cell) == 0) {
		m_cellUpdatables[*cell] = cell;
	}
}

void Cluster::DisableRender(Cell* cell) {
	if (m_cellRenderables.count(cell) > 0) {
		m_cellRenderables.erase(*cell);
	}
}

void Cluster::EnableRender(Cell* cell) {
	if (m_cellRenderables.count(cell) == 0) {
		m_cellRenderables[*cell] = cell;
	}
}


void Cluster::Render(Direct3D* renderer, Camera* cam) {
	for (auto i = m_clusterRenderables.begin(); i != m_clusterRenderables.end(); i++) {
		i->second->Render(renderer, cam);
	}

}


void Cluster::Clean(Hex center) {
	std::vector<Cell*> cleaned;
	// if a cell has no entities, dont update it.
	for (auto i = m_cellUpdatables.begin(); i != m_cellUpdatables.end(); i++) {
		if (i->second->GetEntities()->size() == 0) {
			cleaned.push_back(i->second);
		}
	}
	// perform the removal after the iterator has finished to prevent invalidating the iterator.
	for (int i = 0; i < cleaned.size(); i++) {
		m_cellUpdatables.erase(*cleaned[i]);
	}
	cleaned.clear();
	// if a cell has no visible faces, dont render it.
	for (auto i = m_cellRenderables.begin(); i != m_cellRenderables.end(); i++) {
		if (!CheckRender(i->second)) {
			cleaned.push_back(i->second);
		}
	}

	for (int i = 0; i < cleaned.size(); i++) {
		renderables.erase(*cleaned[i]);
	}

}
