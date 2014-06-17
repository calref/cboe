#include "item.h"

bool item_record_type::isIdent() const
{
	if (item_properties & ITEM_PROP_IDENTIFIED) return true;
	else return false;
}

bool item_record_type::isMagic() const
{
	if (item_properties & ITEM_PROP_MAGIC) return true;
	else return false;
}

bool item_record_type::isContained() const
{
	if (item_properties & ITEM_PROP_CONTAINED) return true;
	else return false;
}

bool item_record_type::isCursed() const
{
	if (item_properties & ITEM_PROP_CURSED) return true;
	else return false;
}

bool item_record_type::isProperty() const
{
	if (item_properties & ITEM_PROP_PROPERTY) return true;
	else return false;
}

bool item_record_type::isConcealed() const
{
	if (item_properties & ITEM_PROP_CONCEALED) return true;
	else return false;
}
