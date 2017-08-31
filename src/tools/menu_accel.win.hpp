
#include <windows.h>
#include <vector>
#include <string>

struct accel_table_t {
	std::vector<ACCEL> table;
	HACCEL handle = NULL;
	void add(WORD cmd, std::string key);
	void build();
	void destroy();
	~accel_table_t();
};