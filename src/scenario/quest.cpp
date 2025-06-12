#include "quest.hpp"

bool cQuest::operator==(const cQuest& other) {
	CHECK_EQ(other, deadline_is_relative);
	CHECK_EQ(other, auto_start);
	CHECK_EQ(other, deadline);
	CHECK_EQ(other, event);
	CHECK_EQ(other, xp);
	CHECK_EQ(other, gold);
	CHECK_EQ(other, bank1);
	CHECK_EQ(other, bank2);
	CHECK_EQ(other, name);
	CHECK_EQ(other, descr);
	return true;
}