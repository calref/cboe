//
//  spell.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-12.
//
//

#ifndef BoE_DATA_SPELL_HPP
#define BoE_DATA_SPELL_HPP

#include <map>
#include <simpletypes.h>

enum eSpellRefer {REFER_YES, REFER_IMMED, REFER_TARGET, REFER_FANCY};
enum eSpellSelect {SELECT_NO, SELECT_ACTIVE, SELECT_ANY};
// This one is meant for indexing a bit field
enum eSpellWhen {WHEN_COMBAT = 1, WHEN_TOWN = 2, WHEN_OUTDOORS = 4};

enum eSpellPat {PAT_SINGLE, PAT_SQ, PAT_SMSQ, PAT_OPENSQ, PAT_RAD2, PAT_RAD3, PAT_PLUS, PAT_WALL};

class cSpell {
	static std::map<eSpell,cSpell> dictionary;
	friend const cSpell& operator*(eSpell spell_num);
public:
	cSpell() {} // This is just here because the map doesn't work without it
	cSpell(eSpell id);
	cSpell& withRefer(eSpellRefer r);
	cSpell& withCost(int c);
	cSpell& withRange(int r);
	cSpell& asLevel(int lvl);
	cSpell& asType(eSkill type);
	cSpell& asPeaceful();
	cSpell& needsSelect(eSpellSelect sel = SELECT_ACTIVE);
	cSpell& when(eSpellWhen when);
	const cSpell& finish();
	eSpell num;
	eSpellRefer refer;
	int cost, range, level;
	eSpellSelect need_select;
	eSkill type;
	int when_cast;
	bool peaceful = false;
	std::string name() const;
	bool is_priest() const;
	static eSpell fromNum(eSkill type, int num);
	static eSpell fromNum(int num);
};

// Need to declare this a second time in order for it to be in scope where it's needed
const cSpell& operator*(eSpell spell_num);

#endif
