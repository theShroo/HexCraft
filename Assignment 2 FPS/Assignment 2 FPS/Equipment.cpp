#include "Equipment.h"
//equipment template directory for predefined equipment.
std::unordered_map<std::string, Equipment*> Equipment::m_templates;

// basic item generator
Equipment* Equipment::GetRandomLoot(int rarity) {
	Equipment* item = new Equipment();
	item->m_slot_required = (rand() % 11) + 1;
	if (item->m_slot_required == off_hand) {
		// no shields for now, lets just add some basic equipment.
		item->m_slot_required = main_hand;
	}
	if (item->m_slot_required == main_hand) {
		// a wepon, lets make one, and call it a sword.
		item->m_optional_slot = off_hand;
		item->m_name = "Mace";
		// if its a weaon we need to give it an attack.
		Damage tempdamage = Damage();

		tempdamage.full_dice = rand()% ((rarity /3)+1);
		tempdamage.partial_dice = 1 + rand() % rarity;
		tempdamage.source = NULL;
		tempdamage.damage_base = "Swing";
		Attack* swing = new Attack{ 10,1,0,1,0,1.0f, tempdamage, "Swing" };
		swing->m_name = "Mace";
		item->m_attacks.push_back(swing);
		item->m_armour_bonus = 0;
		item->m_ammunition = "NULL";

	}
	else if(item->m_slot_required <=11) {
		item->m_name = "armour plates";
		item->m_armour_bonus = (rand() % rarity) + rarity;
	}
	
	return item;

}

Equipment* Equipment::Get(std::string item) {
	if (m_templates.count(item) <= 0) {
		m_templates[item] = new Equipment(item);
	}
	return m_templates[item];
}

void Equipment::Initialise() {
	LoadEquipment();
}
Equipment::Equipment() {
	m_dr = 20;
	m_health = 50;
	m_slot_required = 0;
}
Equipment::Equipment(std::string name) {
	m_name = name;
	m_dr = 20;
	m_health = 50;
	m_slot_required = 0;
	m_ammunition = "NULL";

}

Equipment::~Equipment() {
	for (unsigned i = 0; i < m_attacks.size(); i++) {
		delete m_attacks[i];
	}
	m_attacks.clear();
}

int Equipment::GetDR() {
	return m_armour_bonus;
}

std::vector<Attack*>* Equipment::GetAttacks() {
	return &m_attacks;
}

std::string Equipment::GetName() {
	return m_name;
}

void Equipment::SetOwner(Player* owner) {
	for (unsigned i = 0; i < m_attacks.size(); i++) {
		m_attacks[i]->damage.source = owner;
	}
}

int Equipment::GetDamage(Player* owner) {
	int full = m_attacks[0]->damage.full_dice;
	int part = m_attacks[0]->damage.partial_dice;
	return full * ((rand() % 5) + 1) + part;
}

void Equipment::LoadEquipment() {
	// load pistol into the equipment template directory
	Equipment* item = new Equipment("Pistol");
	item->m_ammunition = "Bullets";
	item->m_slot_required = main_hand;
	Damage tempdamage = Damage();
	tempdamage.full_dice = 3;
	tempdamage.partial_dice = 3;
	tempdamage.source = NULL;
	tempdamage.damage_base = "NULL";
	Attack* shoot = new Attack{ 10,1,0,1,0, 100.0f, tempdamage, "Shoot" };
	shoot->m_name = "Pistol";
	item->m_attacks.push_back(shoot);
	item->m_armour_bonus = 0;
	item->m_ROF = 7.5f;
	m_templates.insert({ item->m_name, item });
	m_templates.insert({"bullets", new Equipment("Bullets")});
	// load fists 
	
	item = new Equipment("Fists");
	item->m_ammunition = "NULL";
	item->m_slot_required = main_hand;
	tempdamage.full_dice = 3;
	tempdamage.partial_dice = 3;
	tempdamage.source = NULL;
	tempdamage.damage_base = "NULL";
	Attack* swing = new Attack{ 10,1,0,1,0, 1.0f, tempdamage, "Punch" };
	swing->m_name = "Fists";
	item->m_attacks.push_back(swing);
	item->m_armour_bonus = 0;
	item->m_ROF = 15.0f;
	m_templates.insert({ item->m_name, item });

}