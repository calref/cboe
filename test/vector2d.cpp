//
//  vector2d.cpp
//  boe_test
//
//  Created by Celtic Minstrel on 2023-01-09.
//

#include "tools/vector2d.hpp"
#include "catch.hpp"

TEST_CASE("vector2d") {
	vector2d<int> test_vector;
	SECTION("default construct") {
		CHECK(test_vector.width() == 0);
		CHECK(test_vector.height() == 0);
	}
	test_vector.resize(5, 5);
	SECTION("assignment") {
		SECTION("by call") {
			test_vector(3,4) = 12;
			CHECK(test_vector(3,4) == 12);
		}
		SECTION("by index") {
			test_vector[3][4] = 12;
			CHECK(test_vector(3,4) == 12);
		}
		SECTION("by row and column") {
			test_vector.row(4)[3] = 12;
			CHECK(test_vector(3,4) == 12);
		}
		SECTION("by column and row") {
			test_vector.col(3)[4] = 12;
			CHECK(test_vector(3,4) == 12);
		}
		SECTION("by row") {
			test_vector.row(4) = {1, 2, 3, 4, 5};
			CHECK(test_vector(0,4) == 1);
			CHECK(test_vector(1,4) == 2);
			CHECK(test_vector(2,4) == 3);
			CHECK(test_vector(3,4) == 4);
			CHECK(test_vector(4,4) == 5);
		}
		SECTION("by column") {
			test_vector.col(4) = {1, 2, 3, 4, 5};
			CHECK(test_vector(4,0) == 1);
			CHECK(test_vector(4,1) == 2);
			CHECK(test_vector(4,2) == 3);
			CHECK(test_vector(4,3) == 4);
			CHECK(test_vector(4,4) == 5);
		}
	}
}

static void fill_some_values(vector2d<int>& tv) {
	tv(1,3) = 1; tv(1,2) = 11; tv(1,4) = 21; tv(1,0) = 31; tv(1,1) = 41;
	tv(2,4) = 2; tv(2,3) = 12; tv(2,2) = 22; tv(2,1) = 32; tv(2,0) = 42;
	tv(0,4) = 3; tv(0,2) = 13; tv(0,1) = 23; tv(0,0) = 33; tv(0,3) = 43;
	tv(3,1) = 4; tv(3,4) = 14; tv(3,0) = 24; tv(3,2) = 34; tv(3,3) = 44;
	tv(4,2) = 5; tv(4,1) = 15; tv(4,4) = 25; tv(4,0) = 35; tv(4,3) = 45;
}

static void check_the_values(const vector2d<int>& tv) {
	CHECK(tv(1,3) == 1); CHECK(tv(1,2) == 11); CHECK(tv(1,4) == 21); CHECK(tv(1,0) == 31); CHECK(tv(1,1) == 41);
	CHECK(tv(2,4) == 2); CHECK(tv(2,3) == 12); CHECK(tv(2,2) == 22); CHECK(tv(2,1) == 32); CHECK(tv(2,0) == 42);
	CHECK(tv(0,4) == 3); CHECK(tv(0,2) == 13); CHECK(tv(0,1) == 23); CHECK(tv(0,0) == 33); CHECK(tv(0,3) == 43);
	CHECK(tv(3,1) == 4); CHECK(tv(3,4) == 14); CHECK(tv(3,0) == 24); CHECK(tv(3,2) == 34); CHECK(tv(3,3) == 44);
	CHECK(tv(4,2) == 5); CHECK(tv(4,1) == 15); CHECK(tv(4,4) == 25); CHECK(tv(4,0) == 35); CHECK(tv(4,3) == 45);
}

TEST_CASE("vector2d resize") {
	vector2d<int> test_vector;
	SECTION("from empty") {
		test_vector.resize(5, 5);
		CHECK(test_vector.width() == 5);
		CHECK(test_vector.height() == 5);
	}
	SECTION("grow width, leave height") {
		test_vector.resize(5, 5);
		fill_some_values(test_vector);
		test_vector.resize(12, 5);
		check_the_values(test_vector);
	}
	SECTION("shrink width, leave height") {
		test_vector.resize(12, 5);
		fill_some_values(test_vector);
		test_vector.resize(5, 5);
		check_the_values(test_vector);
	}
	SECTION("grow width, grow height") {
		test_vector.resize(5, 5);
		fill_some_values(test_vector);
		test_vector.resize(12, 12);
		check_the_values(test_vector);
	}
	SECTION("leave width, grow height") {
		test_vector.resize(5, 5);
		fill_some_values(test_vector);
		test_vector.resize(5, 12);
		check_the_values(test_vector);
	}
	SECTION("shrink width, grow height") {
		test_vector.resize(12, 5);
		fill_some_values(test_vector);
		test_vector.resize(5, 12);
		check_the_values(test_vector);
	}
	SECTION("grow width, shrink height") {
		test_vector.resize(5, 12);
		fill_some_values(test_vector);
		test_vector.resize(12, 5);
		check_the_values(test_vector);
	}
	SECTION("leave width, shrink height") {
		test_vector.resize(5, 12);
		fill_some_values(test_vector);
		test_vector.resize(5, 5);
		check_the_values(test_vector);
	}
	SECTION("shrink width, shrink height") {
		test_vector.resize(12, 12);
		fill_some_values(test_vector);
		test_vector.resize(5, 5);
		check_the_values(test_vector);
	}
}
