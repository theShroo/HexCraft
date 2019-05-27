#ifndef COMBATEXTRAS_H
#define COMBATEXTRAS_H
#include "MathsHelper.h"
// defining all these types inside a circular inclusion caused too many problems, so all combat type, struct and enumeration definitions and declarations are moved here.
// predeclare teh classes involved
// combat results are used to return the agregated numbers that have been involved in a sinlg combat action.
struct CombatResults {
	int damage_taken = 0;
	int DR_applied = 0;
	int dice_rolled = 0;
	int partial_dice_applied = 0;
	int damage_penetration = 1;
	int Skill_roll = 0;
	int hit_location = 0;
	int Critical = 0;
	std::string attack_name = "NULL";

	// requires the addition operator to function so that damage results can be agregated easily;
	CombatResults operator+(CombatResults &other) {
		CombatResults result;
		result.damage_taken = damage_taken + other.damage_taken;
		result.DR_applied = DR_applied + other.DR_applied;
		result.dice_rolled = dice_rolled + other.dice_rolled;
		result.partial_dice_applied = partial_dice_applied + other.partial_dice_applied;
		result.damage_penetration = damage_penetration * other.damage_penetration;
		result.hit_location = hit_location;
		result.attack_name = attack_name;
		result.Skill_roll = Skill_roll;
		result.Critical = Critical;
		return result;
	}

	// and the += operator for ease of use
	CombatResults operator+=(CombatResults &other) {
		damage_taken += other.damage_taken;
		DR_applied += other.DR_applied;
		dice_rolled += other.dice_rolled;
		partial_dice_applied += other.partial_dice_applied;
		damage_penetration *= other.damage_penetration;
		return *this;
	}
};
// damage struct. stores the damage info about a source of damage. including a pointer to the source, in case this is relevent for the purposes
// of determining skill, leech effects or another benifit or cost to the source. including a name for a "base" attack that applying this damage also applies.
// the use of this is currently just to determine the damage bonus from the owner, but will eventually link up to an unordered map that stores a players attacks.
struct Damage {
	int full_dice;
	int partial_dice;
	Player* source = 0;
	std::string damage_base = "None";
	// add comparison operators for the damage struct.

	bool operator<(Damage &otherDamage) {
		if ((full_dice * 3 + partial_dice) < (otherDamage.full_dice * 3 + otherDamage.partial_dice)) {
			return true;
			}
		else {
			return false;
		}
	}
	bool operator>(Damage &otherDamage) {
		if ((full_dice * 3 + partial_dice) > (otherDamage.full_dice * 3 + otherDamage.partial_dice)) {
			return true;
		}
		else {
			return false;
		}
	}

};
// this struct contains the data for a single attack, .
// this allows the use of melee weapons to easily benifit from strenth. later this will also permit the storage of a specific skill used for this attack.
struct Attack {
	int skill;
	int damage_type;
	int armour_penetration_modifier;
	int cycles;
	int cyclic_interval;
	float range;
	Damage damage;
	std::string m_name;
	std::vector<Attack*> follow_up_attacks;

	bool operator<(Attack &otherAttack) {
		if ((damage.full_dice * 3 + damage.partial_dice) < (otherAttack.damage.full_dice * 3 + otherAttack.damage.partial_dice)) {
			return true;
		}
		else {
			return false;
		}
	}
	bool operator>(Attack &otherAttack) {
		if ((damage.full_dice * 3 + damage.partial_dice) > (otherAttack.damage.full_dice * 3 + otherAttack.damage.partial_dice)) {
			return true;
		}
		else {
			return false;
		}
	}
};


// enoumerate equipment slots
enum equipment_slot {
	unequipped = 0,
	main_hand = 1,
	off_hand = 2,
	head = 3,
	neck = 4,
	chest = 5,
	belt = 6,
	back = 7,
	shoulders = 8,
	legs = 9,
	feet = 10,
	hands = 11,
	arms = 12
};
// enumerate damage types, (not yet implemented)
enum damage_types {
	crushing = 1,
	light_piercing = 2,
	piercing = 3,
	heavy_piercing = 4,
	impaling = 5,
	cutting = 6,
	toxic = 7,
	corrosive = 8,
	fire = 9
};

// armour location template, used to calculate armour values for a given location on the body.
struct DamageReductionTemplate {
	int Eyes = 0;
	int Skull = 2;
	int Face = 0;
	int Rleg = 0;
	int Rarm = 0;
	int Torso = 0;
	int Groin = 0;
	int Larm = 0;
	int Lleg = 0;
	int Hand = 0;
	int Foot = 0;
	int Neck = 0;
	int Vitals = 0;
};

// equipment pointer template, used to store the currently equipped items.
struct EquipmentList {
	Equipment* main_hand = 0;
	Equipment* off_hand = 0;
	Equipment* Head = 0;
	Equipment* Belt = 0;
	Equipment* Chest = 0;
	Equipment* Back = 0;
	Equipment* Shoulders = 0;
	Equipment* Neck = 0;
	Equipment* Feet = 0;
	Equipment* Legs = 0;
	Equipment* Arms = 0;
	Equipment* Hands = 0;
};


#endif // !COMBATEXTRAS_H
