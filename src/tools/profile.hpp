#ifndef PROFILE_GLOBAL_H
#define PROFILE_GLOBAL_H

#include <string>
#include <boost/current_function.hpp>

void start_profile(std::string context);
void end_profile(std::string context);
void clear_profile_info();
void print_profile_info();

struct profiler {
	profiler(std::string context) : context(context) {
		start_profile(context);
	}
	~profiler() {
		end_profile(context);
	}
	std::string context;
};

#define PROFILE_FUNC profiler __profiler__(BOOST_CURRENT_FUNCTION)

#endif