//
//  pattern.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-03-03.
//

#include "pattern.hpp"

std::vector<cPattern> cPattern::builtin_patterns;

cPattern::cPattern(const std::string& name, bool rotatable)
	: name(name)
	, rotatable(rotatable)
{
	if(rotatable) {
		new(&patterns) std::vector<effect_pat_type>;
	} else {
		new(&pattern) effect_pat_type{};
	}
}

cPattern::cPattern(const cPattern& other)
	: id(other.id)
	, name(other.name)
	, rotatable(other.rotatable)
{
	if(rotatable) {
		new(&patterns) std::vector<effect_pat_type>(other.patterns);
	} else {
		new(&pattern) effect_pat_type(other.pattern);
	}
}

cPattern::cPattern(cPattern&& other)
	: id(other.id)
	, name(other.name)
	, rotatable(other.rotatable)
{
	if(rotatable) {
		new(&patterns) std::vector<effect_pat_type>(std::move(other.patterns));
	} else {
		new(&pattern) effect_pat_type(std::move(other.pattern));
	}
}

cPattern::~cPattern() {
	if(rotatable) {
		patterns.~vector();
	} else {
		pattern.~array();
	}
}

cPattern& cPattern::with_id(eSpellPat id) {
	this->id = id;
	return *this;
}

cPattern& cPattern::with_pattern(effect_pat_type pat) {
	if(rotatable) {
		patterns.push_back(pat);
	} else {
		pattern = pat;
	}
	return *this;
}

cPattern cPattern::builtin() {
	builtin_patterns.push_back(*this);
	return *this;
}

const cPattern& cPattern::get_builtin(eSpellPat id) {
	for(const auto& pat : builtin_patterns) {
		if(pat.id == id) return pat;
	}
	static cPattern dummy("Null pattern", false);
	return dummy;
}

static const unsigned short X = 0xffff;

cPattern P_SINGLE = cPattern("Single Space", false)
	.with_id(PAT_SINGLE)
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,X,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.builtin();
cPattern P_SQ = cPattern("3x3 Square", false)
	.with_id(PAT_SQ)
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.builtin();
cPattern P_SMSQ = cPattern("2x2 Square", false)
	.with_id(PAT_SMSQ)
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.builtin();
cPattern P_OPENSQ = cPattern("3x3 Open Square", false)
	.with_id(PAT_OPENSQ)
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,X,0,X,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.builtin();
cPattern P_RAD2 = cPattern("Radius 2 Circle", false)
	.with_id(PAT_RAD2)
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,X,X,X,X,X,0,0},
		{0,0,X,X,X,X,X,0,0},
		{0,0,X,X,X,X,X,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.builtin();
cPattern P_RAD3 = cPattern("Radius 3 Circle", false)
	.with_id(PAT_RAD3)
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,X,X,X,X,X,0,0},
		{0,X,X,X,X,X,X,X,0},
		{0,X,X,X,X,X,X,X,0},
		{0,X,X,X,X,X,X,X,0},
		{0,0,X,X,X,X,X,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.builtin();
cPattern P_CROSS = cPattern("Cross", false)
	.with_id(PAT_PLUS)
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,X,0,0,0,0},
		{0,0,0,X,X,X,0,0,0},
		{0,0,0,0,X,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.builtin();
cPattern P_WALL = cPattern("Rotatable 2x8 Wall", true)
	.with_id(PAT_WALL)
	.with_pattern({{
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,X,X,0,0,0}
	}})
	.with_pattern({{
		{0,0,0,0,0,0,0,0,X},
		{0,0,0,0,0,0,0,X,X},
		{0,0,0,0,0,0,X,X,0},
		{0,0,0,0,0,X,X,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,X,X,0,0,0,0,0},
		{0,X,X,0,0,0,0,0,0},
		{X,X,0,0,0,0,0,0,0}
	}})
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{X,X,X,X,X,X,X,X,X},
		{X,X,X,X,X,X,X,X,X},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.with_pattern({{
		{X,0,0,0,0,0,0,0,0},
		{X,X,0,0,0,0,0,0,0},
		{0,X,X,0,0,0,0,0,0},
		{0,0,X,X,0,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,0,X,X,0,0},
		{0,0,0,0,0,0,X,X,0},
		{0,0,0,0,0,0,0,X,X}
	}})
	.with_pattern({{
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,X,X,0,0,0,0}
	}})
	.with_pattern({{
		{0,0,0,0,0,0,0,X,X},
		{0,0,0,0,0,0,X,X,0},
		{0,0,0,0,0,X,X,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,X,X,0,0,0,0,0},
		{0,X,X,0,0,0,0,0,0},
		{X,X,0,0,0,0,0,0,0},
		{X,0,0,0,0,0,0,0,0}
	}})
	.with_pattern({{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{X,X,X,X,X,X,X,X,X},
		{X,X,X,X,X,X,X,X,X},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}
	}})
	.with_pattern({{
		{X,X,0,0,0,0,0,0,0},
		{0,X,X,0,0,0,0,0,0},
		{0,0,X,X,0,0,0,0,0},
		{0,0,0,X,X,0,0,0,0},
		{0,0,0,0,X,X,0,0,0},
		{0,0,0,0,0,X,X,0,0},
		{0,0,0,0,0,0,X,X,0},
		{0,0,0,0,0,0,0,X,X},
		{0,0,0,0,0,0,0,0,X}
	}})
	.builtin();
cPattern P_PROT = cPattern("Protective Circle", false)
	.with_id(PAT_PROT)
	.with_pattern({{
		{0,1,1,1,1,1,1,1,0},
		{1,5,5,5,5,5,5,5,1},
		{1,5,6,6,6,6,6,5,1},
		{1,5,6,3,3,3,6,5,1},
		{1,5,6,3,3,3,6,5,1},
		{1,5,6,3,3,3,6,5,1},
		{1,5,6,6,6,6,6,5,1},
		{1,5,5,5,5,5,5,5,1},
		{0,1,1,1,1,1,1,1,0}
	}})
	.builtin();

