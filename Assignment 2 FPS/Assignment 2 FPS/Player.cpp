#include "Player.h"
#include "Bullets.h"
using namespace DirectX;


Player::Player(XMVECTOR position, std::string shader, std::string mesh, std::string texture, InputController* input) :PhysicsObject(shader, mesh, texture, position)
{
	m_health = 100;
	m_input = input;
	m_distance_traveled = 0.0f;
	m_moveSpeed = 30.0f;
	m_turnSpeed = 3.0f;
	m_ammoCount = L"";
	Loot("Pistol", 1);
	Equip("Pistol");
	Loot("Bullets", 10000);
	m_capsule.SetTop(m_position + m_mesh[0]->GetTop());
	m_capsule.SetBottom(m_position + m_mesh[0]->GetBottom());
	m_capsule.SetRadius(m_mesh[0]->GetRadius());
	m_name = "Player";

	// translate the mesh ofset to center it on the player.
	TranslateOffset(-(m_mesh[0]->GetBottom()),0);
}

void Player::Update(float timestep)
{
	PhysicsObject::Update(timestep);
	m_distance_traveled = 1/ timestep;
	std::wstringstream ss;
	Equipment* ammo = Equipment::Get("Bullets");
	if (m_inventory.count(*ammo) > 0) {
		ss << "Ammo: " << m_inventory[*ammo].second;
	}
	else {
		ss << "Ammo: 0";
	}
	m_ammoCount = ss.str();
	m_cooldown -= timestep;
	XMVECTOR temp = m_position;
	m_capsule.SetBottom(temp);
	temp.m128_f32[1] += m_mesh[0]->GetTop().m128_f32[1];
	m_capsule.SetTop(temp);
}

float Player::GetDistance() {
	return m_distance_traveled;
}

void Player::Loot(std::string loot, int count) {
	Equipment* item = Equipment::Get(loot);
	// if in inventory increment count
	if (m_inventory.count(*item) > 0) {
		m_inventory[*item].second += count;
	}
	else {
		// if not then add the item to the inventory
		m_inventory.insert({ *item, { item,count} });
		if (item->m_ammunition != "NULL") {
			// if its ammo isnt in the inventory then add the ammo type to the inventory, but do not give any ammo.
			Equipment* ammo = Equipment::Get(item->m_ammunition);
			m_inventory.insert({ *ammo,{ammo,0} });
		}
	}
}

int Player::GetItemCount(std::string loot) {
	Equipment* item = Equipment::Get(loot);
	if (m_inventory.count(*item) > 0) {
		return m_inventory[*item].second;
	}
	else {
		return 0;
	}
}

void Player::Equip(std::string loot) {
	Equipment* item = Equipment::Get(loot);
	if (m_inventory.count(*item) > 0) {
		switch (item->m_slot_required) {
		case 1:
			m_equiped_gear.main_hand = item;
			break;
		case 2:
			// no offhand weapons implemeted yet
			break;
		case 3:
			m_equiped_gear.Head = item;
			break;
		case 4:
			m_equiped_gear.Neck = item;
			break;
		case 5:
			m_equiped_gear.Chest = item;
			break;
		case 6:
			m_equiped_gear.Belt = item;
			break;
		case 7:
			m_equiped_gear.Back = item;
			break;
		case 8:
			m_equiped_gear.Shoulders = item;
			break;
		case 9:
			m_equiped_gear.Legs = item;
			break;
		case 10:
			m_equiped_gear.Feet = item;
			break;
		case 11:
			m_equiped_gear.Hands = item;
			break;
		default:
			break;
		}
	}
}

void Player::Unequip(int index) {
	switch (index) {
	case 1:
		m_equiped_gear.main_hand = Equipment::Get("Fists");
		break;
	case 2:
		// no offhand weapons implemeted yet
		break;
	case 3:
		m_equiped_gear.Head = 0;
		break;
	case 4:
		m_equiped_gear.Neck = 0;
		break;
	case 5:
		m_equiped_gear.Chest = 0;
		break;
	case 6:
		m_equiped_gear.Belt = 0;
		break;
	case 7:
		m_equiped_gear.Back = 0;
		break;
	case 8:
		m_equiped_gear.Shoulders = 0;
		break;
	case 9:
		m_equiped_gear.Legs = 0;
		break;
	case 10:
		m_equiped_gear.Feet = 0;
		break;
	case 11:
		m_equiped_gear.Hands = 0;
		break;
	default:
		break;
	}
}



void Player::DoAttack(XMVECTOR direction) {
	if (m_cooldown < 0) {
		Equipment* item = m_equiped_gear.main_hand;
		Attack* attack = item->GetAttacks()->front();
		if (item->m_ammunition == "NULL") {
			// do melee attack here.
		}
		else {
			// ranged attacks
			if (m_inventory[*Equipment::Get(item->m_ammunition)].second > 0) {
				// create a new bullet, these bullets clean themselves up from memeory when they "hit" something so no reference to 
				// these objects needs to be kept here, but a pointer is stored temporarily to change the TTL of the bullet to adjuct range.
				Bullets* a = new Bullets("Unlit Texture Shader",
					"Bullet",
					"Bullet",
					XMVector3Transform(MathsHelper::GetXMVECTOR3(-0.133f, 1.2f, 0.137f), XMMatrixRotationRollPitchYawFromVector(m_rotation)) + m_position,
					XMVector3Normalize(direction)* 0.3f,
					m_rotation);
				// set ttl based on range to set the distance this bullet will travel.
				a->m_ttl = attack->range / 3.0f;
				m_inventory[*Equipment::Get(item->m_ammunition)].second -= 1;
				m_cooldown = item->m_ROF/ 10.0f;
			}
		}
	}
}

// function to provide a description of the item in the specified slot.
std::string Player::GetEquipmentDescription(unsigned index) {
	switch (index) {
	case 0:
		if (m_equiped_gear.main_hand) {
			std::string result;
			result = m_equiped_gear.main_hand->GetName();
			if (m_equiped_gear.main_hand->m_ammunition != "NULL") {
				// append the ammo count to any weapon that uses ammo
				result += " (" + std::to_string(m_inventory[*Equipment::Get(m_equiped_gear.main_hand->m_ammunition)].second) + ")";
			}
			return result;
		}
		else { return "None"; }
		break;
	case 1:
		if (m_equiped_gear.off_hand) {
			return m_equiped_gear.off_hand->GetName();
		}
		else { return "None"; }
		break;
	case 2:
		if (m_equiped_gear.Head) {
			return m_equiped_gear.Head->GetName();
		}
		else { return "None"; }
		break;
	case 3:
		if (m_equiped_gear.Belt) {
			return m_equiped_gear.Belt->GetName();
		}
		else { return "None"; }
		break;
	case 4:
		if (m_equiped_gear.Chest) {
			return m_equiped_gear.Chest->GetName();
		}
		else { return "None"; }
		break;
	case 5:
		if (m_equiped_gear.Back) {
			return m_equiped_gear.Back->GetName();
		}
		else { return "None"; }
		break;
	case 6:
		if (m_equiped_gear.Shoulders) {
			return m_equiped_gear.Shoulders->GetName();
		}
		else { return "None"; }
		break;
	case 7:
		if (m_equiped_gear.Neck) {
			return m_equiped_gear.Neck->GetName();
		}
		else { return "None"; }
		break;
	case 8:
		if (m_equiped_gear.Feet) {
			return m_equiped_gear.Feet->GetName();
		}
		else { return "None"; }
		break;
	case 9:
		if (m_equiped_gear.Legs) {
			return m_equiped_gear.Legs->GetName();
		}
		else { return "None"; }
		break;
	case 10:
		if (m_equiped_gear.Arms) {
			return m_equiped_gear.Arms->GetName();
		}
		else { return "None"; }
		break;
	case 11:
		if (m_equiped_gear.Hands) {
			return m_equiped_gear.Hands->GetName();
		}
		else { return "None"; }
		break;
	default:
		return "error!";
	}
}