/*
 *  container.hpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 2020-12-21.
 *
 */

#ifndef BOE_DIALOG_CONTAINER_HPP
#define BOE_DIALOG_CONTAINER_HPP

#include "control.hpp"

typedef std::map<std::string,cControl*>::iterator ctrlIter;

/// A superclass to represent a control that contains other controls.
class cContainer : public cControl, public iNameGiver {
	void callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) override;
protected:
	std::string clicking;
	/// Parses a child control.
	/// @param elem The element defining the control.
	/// @param controls The map into which the control will be inserted.
	/// @param[out] The ID of the new control.
	/// @return true if the element was a valid control, false otherwise.
	bool parseChildControl(ticpp::Element& elem, std::map<std::string,cControl*>& controls, std::string& id, std::string fname);
public:
	/// Create a new container control.
	/// @param t The type of the control.
	/// @param p The parent.
	cContainer(eControlType t, iComponent& p) : cControl(t, p) {}
	/// Check if a control exists with a given ID.
	/// @param id The unique key of the control.
	/// @return true if it exists.
	virtual bool hasChild(std::string id) const = 0;
	/// Get a reference to a child control.
	/// @param id The unique key of the control.
	/// @throw std::invalid_argument if the control does not exist.
	/// @return a reference to the requested control.
	virtual cControl& getChild(std::string id) = 0;
	/// Executes a function for every control in this container.
	/// @param callback A function taking a string as its first argument
	/// and a control reference as its second argument.
	virtual void forEach(std::function<void(std::string,cControl&)> callback) = 0;
	/// Generate a unique ID for a child control. The explicitId is the ID specified in the XML, if any.
	/// This may be called more than once, so it should not return the same value twice in a row,
	/// unless it can guarantee the value is not already assigned to another control.
	virtual std::string generateId(const std::string& explicitId) const override;
	virtual void postChildrenResolve() {
		recalcRect();
	}
	/// @copydoc getChild()
	cControl& operator[](std::string id) {return getChild(id);}
	const cControl& operator[](std::string id) const {return const_cast<cContainer&>(*this).getChild(id);}
	bool isContainer() const override {return true;}
	bool handleClick(location where, cFramerateLimiter& fps_limiter) override;
	void validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) override;
private:
	std::pair<std::string,cControl*> prevCtrl{"", nullptr};
};

#endif
