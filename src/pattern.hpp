//
//  pattern.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-03-03.
//

#ifndef BoE_DATA_PATTERN_HPP
#define BoE_DATA_PATTERN_HPP

#include <array>
#include <vector>

enum eSpellPat {
	PAT_SINGLE,
	PAT_SQ, PAT_SMSQ, PAT_OPENSQ,
	PAT_RAD2, PAT_RAD3, PAT_PLUS,
	PAT_WALL,
	PAT_PROT = PAT_WALL + 8,
	PAT_CUSTOM,
	PAT_CURRENT = -1
};

using effect_pat_type = std::array<std::array<unsigned short, 9>, 9>;

class cPattern {
	static std::vector<cPattern> builtin_patterns;
public:
	eSpellPat id = PAT_CUSTOM;
	std::string name;
	const bool rotatable = false;
	union {
		effect_pat_type pattern;
		std::vector<effect_pat_type> patterns;
	};
	cPattern(const std::string& name, bool rotatable);
	cPattern(const cPattern&);
	cPattern(cPattern&&);
	cPattern& with_id(eSpellPat id);
	cPattern& with_pattern(effect_pat_type pat);
	cPattern builtin();
	~cPattern();
	static const cPattern& get_builtin(eSpellPat id);
};

std::ostream& operator<< (std::ostream& out, eSpellPat pat);
std::istream& operator>> (std::istream& in, eSpellPat& pat);

#endif
