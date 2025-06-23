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
	void setPict(pic_num_t num, ePicType type);
	/// Set the pict's icon.
	/// @param num The new icon index.
	void setPict(pic_num_t num);
	/// Automatically recalculate the icon's bounding rect based on its current picture.
	void recalcRect() override;
	/// Get the current icon.
	/// @return The number of the current icon.
	pic_num_t getPicNum() const;
	/// Get the current icon's type.
	/// @return The type of the current icon.
	ePicType getPicType() const;
	/// Create a new icon.
	/// @param parent The parent.
	explicit cPict(iComponent& parent);
	bool isClickable() const override;
	bool isFocusable() const override;
	bool isScrollable() const override;
	/// Advance the current animation frame.
	/// Should be called at predictable intervals if the dialog might contain an animated graphic.
	static void advanceAnim();
	virtual ~cPict();
	void draw() override;
	void draw(sf::RenderTarget& target);
	/// A utility function to draw an icon into an arbitrary window.
	/// @param win The window to draw in.
	/// @param dest The bounding rect to draw in (ignored for drawing the actual, but used for background fill and framing)
	/// @param which_g The icon to draw.
	/// @param type_g The type of icon to draw.
	/// @param framed Whether to draw a frame around the icon.
	static void drawAt(sf::RenderWindow& win, rectangle dest, pic_num_t which_g, ePicType type_g, bool framed);
	static void drawAt(sf::RenderWindow& win, sf::RenderTarget& targ, rectangle dest, pic_num_t which_g, ePicType type_g, bool framed);
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
	inline static void resetAnim() { animFrame = 0; }
	void setAnimLoops(short value);
private:
	static std::shared_ptr<const sf::Texture> getSheetInternal(eSheetType type, size_t n);
	std::shared_ptr<const sf::Texture> getSheet(eSheetType type, size_t n = 0);
	std::shared_ptr<const sf::Texture> sheetCached;
	eSheetType sheetCachedType;
	size_t sheetCachedNum;
	static short animFrame;
	short animLoops = -1;
	short prevAnimFrame = 0;
	void updateAnim(short loop_frames);
	pic_num_t picNum;
	ePicType picType;
	bool drawScaled;
	sf::Color fillClr = sf::Color::Black;
	// Transient parse flags
	bool wide = false, tall = false, tiny = false, custom = false, blank = false, filled = true;
	void drawPresetTer(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetTerAnim(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetMonstSm(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetMonstWide(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetMonstTall(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetMonstLg(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetDlog(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetDlogLg(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetTalk(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetScen(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetScenLg(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetItem(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetTinyItem(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetPc(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetField(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetBoom(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetMissile(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPresetTerMap(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawStatusIcon(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawFullSheet(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomTer(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomTerAnim(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomMonstSm(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomMonstWide(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomMonstTall(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomMonstLg(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomDlog(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomDlogLg(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomTalk(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomItem(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomTinyItem(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomBoom(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomMissile(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawCustomTerMap(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPartyMonstSm(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPartyMonstWide(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPartyMonstTall(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPartyMonstLg(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPartyScen(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPartyItem(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawPartyPc(short num, rectangle to_rect, sf::RenderTarget& targ);
	void drawInvenBtn(short num, rectangle to_rect, sf::RenderTarget& targ);
	static std::map<ePicType,void(cPict::*)(short,rectangle,sf::RenderTarget&)>& drawPict();
};

#endif
