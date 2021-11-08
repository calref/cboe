//
//  fields.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_FIELDS_HPP
#define BoE_FIELDS_HPP

// This is a slight misnomer, as a couple of these are not true fields.
enum eFieldType {
	SPECIAL_EXPLORED = 0,
	WALL_FORCE = 1,
	WALL_FIRE = 2,
	FIELD_ANTIMAGIC = 3,
	CLOUD_STINK = 4,
	WALL_ICE = 5,
	WALL_BLADES = 6,
	CLOUD_SLEEP = 7,
	// Begin fields saved in town setup
	OBJECT_BLOCK = 8,
	SPECIAL_SPOT = 9, // Space contains a white special spot
	FIELD_WEB = 10,
	OBJECT_CRATE = 11,
	OBJECT_BARREL = 12,
	BARRIER_FIRE = 13,
	BARRIER_FORCE = 14,
	FIELD_QUICKFIRE = 15,
	// End fields saved in town setup
	SFX_SMALL_BLOOD = 16,
	SFX_MEDIUM_BLOOD = 17,
	SFX_LARGE_BLOOD = 18,
	SFX_SMALL_SLIME = 19,
	SFX_LARGE_SLIME = 20,
	SFX_ASH = 21,
	SFX_BONES = 22,
	SFX_RUBBLE = 23,
	BARRIER_CAGE = 24,
	SPECIAL_ROAD = 25,
	// From here on are special values that don't index anything.
	// Thus, they start at 32.
	FIELD_DISPEL = 32, // Dispel field
	FIELD_SMASH = 33, // Move Mountains
	// Mustn't have anything >= 50
};

// Field types are used to index bit fields.
// In this case, their values should be taken as a bit index that is set.
inline unsigned long operator&(unsigned long a, eFieldType b) {
	return a & (1 << b);
}
inline unsigned long operator&(eFieldType a, unsigned long b) {
	return (1 << a) & b;
}
inline unsigned long operator&(eFieldType a, eFieldType b) {
	return (1 << a) & (1 << b);
}
inline unsigned long& operator &=(unsigned long& a, eFieldType b) {
	a = a & b;
	return a;
}
inline unsigned long operator|(unsigned long a, eFieldType b) {
	return a | (1 << b);
}
inline unsigned long operator|(eFieldType a, unsigned long b) {
	return (1 << a) | b;
}
inline unsigned long operator|(eFieldType a, eFieldType b) {
	return (1 << a) | (1 << b);
}
inline unsigned long& operator |=(unsigned long& a, eFieldType b) {
	a = a | b;
	return a;
}
inline unsigned long operator^(unsigned long a, eFieldType b) {
	return a ^ (1 << b);
}
inline unsigned long operator^(eFieldType a, unsigned long b) {
	return (1 << a) ^ b;
}
inline unsigned long operator^(eFieldType a, eFieldType b) {
	return (1 << a) ^ (1 << b);
}
inline unsigned long& operator ^=(unsigned long& a, eFieldType b) {
	a = a ^ b;
	return a;
}
inline unsigned long operator>>(eFieldType a, unsigned long b) {
	return (1 << a) >> b;
}
inline unsigned long operator<<(eFieldType a, unsigned long b) {
	return (1 << a) << b;
}
inline unsigned long operator~(eFieldType f) {
	return ~(1 << f);
}

std::ostream& operator << (std::ostream& out, eFieldType e);
std::istream& operator >> (std::istream& in, eFieldType& e);

#endif
