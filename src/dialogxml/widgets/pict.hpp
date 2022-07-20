/*
 *  pict.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef PICT_H
#define PICT_H

/// @file
/// Icon-related classes and types.

#include <SFML/Graphics.hpp>

#include <vector>
#include <map>
#include <memory>
#include "control.hpp"
#include "pictypes.hpp"
#include "gfx/texture.hpp"

class cPictNum {
public:
	cPictNum(pic_num_t nNum=-1, ePicType nType=ePicType::PIC_NONE)
		: num(nNum)
		, type(nType)
	{
	}
	bool operator==(const cPictNum&pict) const { return num==pict.num && type == pict.type; }
	bool operator!=(const cPictNum&pict) const { return !operator==(pict);}
	pic_num_t num;
	ePicType type;
};

std::ostream& operator<< (std::ostream& out, cPictNum pic);
std::istream& operator>> (std::istream& in, cPictNum& pic);

/// A simple icon.
/// This control can also be made clickable.
class cPict : public cControl {
public:
	/// @copydoc cDialog::init()
	static void init();
	bool parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) override;
	void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) override;
	bool manageFormat(eFormat prop, bool set, boost::any* val) override;
	storage_t store() const override;
	void restore(storage_t to) override;
	/// @copydoc setPict(pic_num_t)
	/// @param type The type of the new icon
	/// @note Calling this function automatically adjusts the bounding rect so that the picture fits perfectly.
	/// It does not change the control's position.
	///
	/// This function applies a few automatic adjustments to its input:
	///
	/// - If type is PIC_MONST, it automatically looks up the chosen icon to determine
	/// whether it should apply the tall or wide modifiers.
	/// - If num is 4 digits in decimal and type is not PIC_FULL, it automatically takes the remainder by 1000
	/// and applies the custom modifier. (If type is PIC_TER_MAP, it does not take the remainder by 1000.)
	/// - If num is 10000 or greater and type is PIC_TER_MAP, it automatically subtracts 10000 and applies the party modifier.
	void setPict(pic_num_t num, ePicType type, bool updateResType=true) {
		setPict(cPictNum(num, type), updateResType);
	}
	void setPict(cPictNum const &num, bool updateResType=false) {
		picture.num=num.num;
		if (updateResType)
			resultType=num.type;
		else
			picture.type=num.type;
		updateResultType(num.type);
	}
	/// Automatically recalculate the icon's bounding rect based on its current picture.
	void recalcRect() override;
	/// Get the current icon.
	/// @return The number of the current icon.
	pic_num_t getPicNum() const;
	/// Get the current icon's type.
	/// @return The type of the current icon.
	ePicType getPicType() const;
	/// Create a new icon.
	/// @param parent The parent dialog.
	explicit cPict(cDialog& parent);
	/// Create a new icon without a parent dialog.
	/// @param parent The parent window.
	explicit cPict(sf::RenderWindow& parent);
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	/// Advance the current animation frame.
	/// Should be called at predictable intervals if the dialog might contain an animated graphic.
	static void advanceAnim();
	virtual ~cPict();
	void draw() override;
	// only implemented for item and terrain pictures
	static bool get_picture(cPictNum pict, Texture &source, rectangle &from_rect, int anim=0, int which_part=0);
	/// A utility function to draw an icon into an arbitrary window.
	/// @param win The window to draw in.
	/// @param dest The bounding rect to draw in (ignored for drawing the actual, but used for background fill and framing)
	/// @param which_g The icon to draw.
	/// @param type_g The type of icon to draw.
	/// @param framed Whether to draw a frame around the icon.
	static void drawAt(sf::RenderWindow& win, rectangle dest, pic_num_t which_g, ePicType type_g, bool framed);
	/// A convenience constant that can be passed as the pic number to setPict(pic_num_t num).
	/// It sets the icon to nothing, showing as just black.
	static const pic_num_t BLANK;
	/// @copydoc cControl::getSupportedHandlers
	///
	/// @todo Document possible handlers
	std::set<eDlogEvt> getSupportedHandlers() const override {
		return {EVT_CLICK};
	}
	cPict& operator=(cPict& other) = delete;
	cPict(cPict& other) = delete;
private:
	static std::shared_ptr<const Texture> getSheetInternal(eSheetType type, size_t n);
	std::shared_ptr<const Texture> getSheet(eSheetType type, size_t n = 0);
	std::shared_ptr<const Texture> sheetCached;
	eSheetType sheetCachedType;
	size_t sheetCachedNum;
	static short animFrame;
	cPictNum picture;
	ePicType resultType;
	bool drawScaled;
	// Transient parse flags
	bool wide = false, tall = false, tiny = false, custom = false, blank = false;
	void updateResultType(ePicType type);
	void drawPresetTer(rectangle to_rect);
	void drawPresetTerAnim(rectangle to_rect);
	void drawPresetMonstSm(rectangle to_rect);
	void drawPresetMonstWide(rectangle to_rect);
	void drawPresetMonstTall(rectangle to_rect);
	void drawPresetMonstLg(rectangle to_rect);
	void drawPresetDlog(rectangle to_rect);
	void drawPresetDlogLg(rectangle to_rect);
	void drawPresetTalk(rectangle to_rect);
	void drawPresetScen(rectangle to_rect);
	void drawPresetScenLg(rectangle to_rect);
	void drawPresetItem(rectangle to_rect);
	void drawPresetTinyItem(rectangle to_rect);
	void drawPresetPc(rectangle to_rect);
	void drawPresetField(rectangle to_rect);
	void drawPresetBoom(rectangle to_rect);
	void drawPresetMissile(rectangle to_rect);
	void drawPresetTerMap(rectangle to_rect);
	void drawStatusIcon(rectangle to_rect);
	void drawFullSheet(rectangle to_rect);
	void drawCustomTer(rectangle to_rect);
	void drawCustomTerAnim(rectangle to_rect);
	void drawCustomMonstSm(rectangle to_rect);
	void drawCustomMonstWide(rectangle to_rect);
	void drawCustomMonstTall(rectangle to_rect);
	void drawCustomMonstLg(rectangle to_rect);
	void drawCustomDlog(int num, rectangle to_rect);
	void drawCustomDlog(rectangle to_rect);
	void drawCustomDlogLg(rectangle to_rect);
	void drawCustomTalk(rectangle to_rect);
	void drawCustomItem(rectangle to_rect);
	void drawCustomTinyItem(rectangle to_rect);
	void drawCustomBoom(rectangle to_rect);
	void drawCustomMissile(rectangle to_rect);
	void drawCustomTerMap(rectangle to_rect);
	void drawPartyMonstSm(rectangle to_rect);
	void drawPartyMonstWide(rectangle to_rect);
	void drawPartyMonstTall(rectangle to_rect);
	void drawPartyMonstLg(rectangle to_rect);
	void drawPartyScen(rectangle to_rect);
	void drawPartyItem(rectangle to_rect);
	void drawPartyPc(rectangle to_rect);
	cPictNum getSourcePicture(ePicType defaultType) const {
		cPictNum res=picture;
		if (res.type==ePicType::PIC_NONE) res.type=defaultType;
		return res;
	}
	static std::map<ePicType,void(cPict::*)(rectangle)>& drawPict();
};

#endif
