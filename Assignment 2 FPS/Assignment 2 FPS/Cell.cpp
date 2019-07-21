#include "Cell.h"
#include "PhysicsObject.h"
#include "Map.h"

using namespace DirectX;
using namespace HexLogic;

Cell::Cell(XMVECTOR position, Cluster* owner) {
	m_type = "Air";
	m_position = position;
	m_location = VectorToHex(m_position);
	m_gameObject = new GameObject(position, "Diffuse Texture Fog Shader", "Hexagon", m_type);
	m_gameObject->Update(0.0f);
	m_passable = true;
	m_health = 1;
	m_solid = false;
	m_initialised = &Cell::_GetNeigboursInit;
	m_cluster = owner;
	m_owner = owner->GetOwner();
}

// smartptrs remove the need for manual deletion of many game objects and manual pointers

Cell::~Cell() {
	delete m_gameObject;
}

void Cell::Render(Direct3D* renderer, Camera* cam) {
	m_gameObject->Render(renderer, cam);
}


// return cell type for game logic
std::string Cell::GetType() {
	return m_type;
}

void Cell::SetType(std::string new_type, bool passable, bool solid) {
	m_type = new_type;
	m_passable = passable;
	m_health = 1;
	m_gameObject->SetTexture(m_type);
	m_gameObject->Update(0.0f);
	m_solid = solid;
}


void Cell::Break(int damage, int penetration) {
	m_health -= damage - (m_resistance - penetration);
	Update(0);
}
// cells need to update their data, especially if their tile has been destroyed.
// little optimisation here to avoid initialising the object more than once, and to avoid the if statement
// we will use a function pointer to the correct update version to run.

void Cell::Initialise() {
	m_initialised = &Cell::_GetNeigbours;
	std::vector<Hex> neigbours;	
	m_owner->GetLocalMap(neigbours, m_location, 1);
	for (int i = 0, j = 0; i < neigbours.size(); i++, j++) {
		if( neigbours[i] != m_location){
			m_neighbours[j] = m_owner->GetCell(neigbours[i]);
		}
		else { j--; }
	}
	
}

void Cell::Update(float timestep) {
	if (m_health <= 0) {
		GameObject* loot = m_owner->AddLoot(m_type, "Diffuse Texture Fog Shader", "Hexagon", m_type, m_position, 1);
		SetType("Air", true, false);
		m_owner->RenderCheck(m_location);
		loot->Resize(0.1f);
	}
}

Hex Cell::GetLocation(){
	return m_location;
}

WeakCellPtr* Cell::GetNeigbours() {
	return (this->*m_initialised)();
}

WeakCellPtr* Cell::_GetNeigbours() {
	return m_neighbours;
}

WeakCellPtr* Cell::_GetNeigboursInit() {
	Initialise();
	return m_neighbours;
}

bool Cell::CheckRender() {
	bool renderable = false;
	// this brute force render check replaces the math heavy version that only checks the cells on the visible faces,
	// the result is that cells that have no faces visible to the player are still rendered, BUT this is still twice as efficient compared to the other version.
	if (!renderable && IsSolid()) {
		for (int i = 0; i < 20 && !renderable; i++) {
			CellPtr current = GetNeigbours()[i].lock();
			if (!current) {
				Initialise();
			}
			if (!current->IsSolid()) {
				renderable = true;
			}
		}
	}
	return renderable;
}

void Cell::Uninitialise()
{
	m_initialised = &Cell::_GetNeigboursInit;
}

CellPtr Cell::operator[](int index) {
	CellPtr target = m_neighbours[index].lock();
	if (target) return target;
	Initialise();
	return m_neighbours[index].lock();
}