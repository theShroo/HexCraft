/*	Equipment class written by Russell Chidambaranathan 2017.
*
*	Equipment manager and texture class,
*	this class provides simple accessor and loader functions to simplify the process of managing
*	equipment.
*	
*/


#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "CombatExtras.h"

// a class to handle equipment things.



class Equipment {
public:
	static Equipment* GetRandomLoot(int rarity = 1);
	static Equipment* Get(std::string item);
	static void Initialise();
	~Equipment();
	int GetDR();
	int m_slot_required;
	int m_optional_slot;
	std::vector<Attack*>* GetAttacks();
	std::string GetName();
	void SetOwner(Player* owner);
	operator PointerKey() { return PointerKey{ reinterpret_cast<size_t>(this) }; }
	std::string m_ammunition;
	float m_ROF;
	int GetDamage(Player* owner);

private:
	static void LoadEquipment();
	static	std::unordered_map<std::string, Equipment*> m_templates;
	Equipment();
	Equipment(std::string name);
	std::vector<Attack*> m_attacks;
	int m_dr;
	int m_health;
	std::string m_name;
	int m_armour_bonus;

};

#endif // !EQUIPMENT_H
