/*
 *  item.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "location.h"

namespace legacy { struct item_record_type; };

/*      items[i].type    a.k.a type of weapon         */
enum eWeapType {
	ITEM_NOT_MELEE = 0,
	ITEM_EDGED = 1,
	ITEM_BASHING = 2,
	ITEM_POLE = 3,
};

/*      items[i].variety    a.k.a item type (in editor)      */
enum eItemType {
	ITEM_TYPE_NO_ITEM = 0,
	ITEM_TYPE_ONE_HANDED = 1,
	ITEM_TYPE_TWO_HANDED = 2,
	ITEM_TYPE_GOLD = 3,
	ITEM_TYPE_BOW = 4,
	ITEM_TYPE_ARROW = 5,
	ITEM_TYPE_THROWN_MISSILE = 6,
	ITEM_TYPE_POTION = 7, // potion/magic item
	ITEM_TYPE_SCROLL = 8, // scroll/magic item
	ITEM_TYPE_WAND = 9,
	ITEM_TYPE_TOOL = 10,
	ITEM_TYPE_FOOD = 11,
	ITEM_TYPE_SHIELD = 12,
	ITEM_TYPE_ARMOR = 13,
	ITEM_TYPE_HELM = 14,
	ITEM_TYPE_GLOVES = 15,
	ITEM_TYPE_SHIELD_2 = 16,
	// don't know why a second type of shield is used ; it is actually checked
	// in the armor code (item >= 12 and <= 17)
	// and you can't equip another (12) shield while wearing it ... I didn't
	// find a single item with this property in the bladbase.exs ...
	ITEM_TYPE_BOOTS = 17,
	ITEM_TYPE_RING = 18,
	ITEM_TYPE_NECKLACE = 19,
	ITEM_TYPE_WEAPON_POISON = 20,
	ITEM_TYPE_NON_USE_OBJECT = 21,
	ITEM_TYPE_PANTS = 22,
	ITEM_TYPE_CROSSBOW = 23,
	ITEM_TYPE_BOLTS = 24,
	ITEM_TYPE_MISSILE_NO_AMMO = 25, //e.g slings
	ITEM_TYPE_UNUSED1 = 26, // these are here solely because they are options in the editor
	ITEM_TYPE_UNUSED2 = 27,
};

/*      items[i].ability      */
enum eItemAbil {
	// Weapon abilities
	ITEM_NO_ABILITY = 0,
	ITEM_FLAMING_WEAPON = 1,
	ITEM_DEMON_SLAYER = 2,
	ITEM_UNDEAD_SLAYER = 3,
	ITEM_LIZARD_SLAYER = 4,
	ITEM_GIANT_SLAYER = 5,
	ITEM_MAGE_SLAYER = 6,
	ITEM_PRIEST_SLAYER = 7,
	ITEM_BUG_SLAYER = 8,
	ITEM_ACIDIC_WEAPON = 9,
	ITEM_SOULSUCKER = 10,
	ITEM_DRAIN_MISSILES = 11,
	ITEM_WEAK_WEAPON = 12,
	ITEM_CAUSES_FEAR = 13,
	ITEM_POISONED_WEAPON = 14,
	// General abilities
	ITEM_PROTECTION = 30,
	ITEM_FULL_PROTECTION = 31,
	ITEM_FIRE_PROTECTION = 32,
	ITEM_COLD_PROTECTION = 33,
	ITEM_POISON_PROTECTION = 34,
	ITEM_MAGIC_PROTECTION = 35,
	ITEM_ACID_PROTECTION = 36,
	ITEM_SKILL = 37,
	ITEM_STRENGTH = 38,
	ITEM_DEXTERITY = 39,
	ITEM_INTELLIGENCE = 40,
	ITEM_ACCURACY = 41,
	ITEM_THIEVING = 42,
	ITEM_GIANT_STRENGTH = 43,
	ITEM_LIGHTER_OBJECT = 44,
	ITEM_HEAVIER_OBJECT = 45,
	ITEM_OCCASIONAL_BLESS = 46,
	ITEM_OCCASIONAL_HASTE = 47,
	ITEM_LIFE_SAVING = 48,
	ITEM_PROTECT_FROM_PETRIFY = 49,
	ITEM_REGENERATE = 50,
	ITEM_POISON_AUGMENT = 51,
	ITEM_DISEASE_PARTY = 52,
	ITEM_WILL = 53,
	ITEM_FREE_ACTION = 54,
	ITEM_SPEED = 55,
	ITEM_SLOW_WEARER = 56,
	ITEM_PROTECT_FROM_UNDEAD = 57,
	ITEM_PROTECT_FROM_DEMONS = 58,
	ITEM_PROTECT_FROM_HUMANOIDS = 59,
	ITEM_PROTECT_FROM_REPTILES = 60,
	ITEM_PROTECT_FROM_GIANTS = 61,
	ITEM_PROTECT_FROM_DISEASE = 62,
	// Nonspell Usable
	ITEM_POISON_WEAPON = 70, //put poison on weapon
	ITEM_BLESS_CURSE = 71,
	ITEM_AFFECT_POISON = 72,
	ITEM_HASTE_SLOW = 73,
	ITEM_AFFECT_INVULN = 74,
	ITEM_AFFECT_MAGIC_RES = 75,
	ITEM_AFFECT_WEB = 76,
	ITEM_AFFECT_DISEASE = 77,
	ITEM_AFFECT_SANCTUARY = 78,
	ITEM_AFFECT_DUMBFOUND = 79,
	ITEM_AFFECT_MARTYRS_SHIELD = 80,
	ITEM_AFFECT_SLEEP = 81,
	ITEM_AFFECT_PARALYSIS = 82,
	ITEM_AFFECT_ACID = 83,
	ITEM_BLISS = 84,
	ITEM_AFFECT_EXPERIENCE = 85,
	ITEM_AFFECT_SKILL_POINTS = 86,
	ITEM_AFFECT_HEALTH = 87,
	ITEM_AFFECT_SPELL_POINTS = 88,
	ITEM_DOOM = 89,
	ITEM_LIGHT = 90,
	ITEM_STEALTH = 91,
	ITEM_FIREWALK = 92,
	ITEM_FLYING = 93,
	ITEM_MAJOR_HEALING = 94,
	// Spell Usable
	ITEM_SPELL_FLAME = 110,
	ITEM_SPELL_FIREBALL = 111,
	ITEM_SPELL_FIRESTORM = 112,
	ITEM_SPELL_KILL = 113,
	ITEM_SPELL_ICE_BOLT = 114,
	ITEM_SPELL_SLOW = 115,
	ITEM_SPELL_SHOCKWAVE = 116,
	ITEM_SPELL_DISPEL_UNDEAD = 117,
	ITEM_SPELL_DISPEL_SPIRIT = 118,
	ITEM_SPELL_SUMMONING = 119,
	ITEM_SPELL_MASS_SUMMONING = 120,
	ITEM_SPELL_ACID_SPRAY = 121,
	ITEM_SPELL_STINKING_CLOUD = 122,
	ITEM_SPELL_SLEEP_FIELD = 123,
	ITEM_SPELL_VENOM = 124,
	ITEM_SPELL_SHOCKSTORM = 125,
	ITEM_SPELL_PARALYSIS = 126,
	ITEM_SPELL_WEB_SPELL = 127,
	ITEM_SPELL_STRENGTHEN_TARGET = 128, //wand of carrunos effect
	ITEM_SPELL_QUICKFIRE = 129,
	ITEM_SPELL_MASS_CHARM = 130,
	ITEM_SPELL_MAGIC_MAP = 131,
	ITEM_SPELL_DISPEL_BARRIER = 132,
	ITEM_SPELL_MAKE_ICE_WALL = 133,
	ITEM_SPELL_CHARM_SPELL = 134,
	ITEM_SPELL_ANTIMAGIC_CLOUD = 135,
	// Reagents
	ITEM_HOLLY = 150, // Holly/Toadstool
	ITEM_COMFREY_ROOT = 151,
	ITEM_GLOWING_NETTLE = 152,
	ITEM_WORMGRASS = 153, // Crypt Shroom/Wormgr.
	ITEM_ASPTONGUE_MOLD = 154,
	ITEM_EMBER_FLOWERS = 155,
	ITEM_GRAYMOLD = 156,
	ITEM_MANDRAKE = 157,
	ITEM_SAPPHIRE = 158,
	ITEM_SMOKY_CRYSTAL = 159,
	ITEM_RESSURECTION_BALM = 160,
	ITEM_LOCKPICKS = 161,
	// Missile Abilities
	ITEM_MISSILE_RETURNING = 170,
	ITEM_MISSILE_LIGHTNING = 171,
	ITEM_MISSILE_EXPLODING = 172,
	ITEM_MISSILE_ACID = 173,
	ITEM_MISSILE_SLAY_UNDEAD = 174,
	ITEM_MISSILE_SLAY_DEMON = 175,
	ITEM_MISSILE_HEAL_TARGET = 176,
};

class cItemRec {
public:
	eItemType variety;
	short item_level;
	char awkward;
	char bonus;
	char protection;
	char charges;
	eWeapType type;
	char magic_use_type;
	unsigned short graphic_num;
	eItemAbil ability;
	unsigned char ability_strength;
	unsigned char type_flag;
	unsigned char is_special;
	short value;
	unsigned char weight;
	unsigned char special_class;
	location item_loc;
	string full_name;
	string name;
	unsigned char treas_class;
	unsigned char item_properties;
private:
	unsigned char reserved1;
	unsigned char reserved2;
public:
	//string desc; // for future use
	unsigned char rec_treas_class() const;
	bool is_ident() const;
	bool is_property() const;
	bool is_magic() const;
	bool is_contained() const;
	bool is_cursed() const;
	bool is_concealed() const;
	bool is_enchanted() const;
	void set_ident(bool b);
	void set_property(bool b);
	void set_magic(bool b);
	void set_contained(bool b);
	void set_cursed(bool b);
	void set_concealed(bool b);
	void set_enchanted(bool b);
	short item_weight() const;
	
	cItemRec();
	cItemRec(long preset);
	cItemRec& operator = (legacy::item_record_type& old);
	void writeTo(ostream& file, string prefix = "");
	void readAttrFrom(string cur, istream& sin);
};

ostream& operator << (ostream& out, eWeapType& e);
ostream& operator << (ostream& out, eItemType& e);
ostream& operator << (ostream& out, eItemAbil& e);
istream& operator >> (istream& in, eWeapType& e);
istream& operator >> (istream& in, eItemType& e);
istream& operator >> (istream& in, eItemAbil& e);

/*
 typedef struct {
 short variety, item_level;
 char awkward, bonus, protection, charges, type;
 unsigned char graphic_num,ability, type_flag, is_special;
 short value;
 bool identified, magic;
 unsigned char weight, description_flag;
 char full_name[25], name[15];
 unsigned char reserved1,reserved2;
 unsigned char magic_use_type, ability_strength, treas_class, real_abil;
 } short_item_record_type;
 */
