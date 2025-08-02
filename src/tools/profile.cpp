#include "profile.hpp"

#include <vector>
#include <map>
#include <algorithm>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

std::map<std::string, sf::Time> context_timers;
std::vector<std::string> context_stack;
sf::Clock current_context_timer;

void start_profile(std::string context) {
	if(!context_stack.empty()){
		context_timers[context_stack.back()] += current_context_timer.restart();
	}
	if(context_timers.find(context) == context_timers.end()){
		sf::Time context_timer;
		context_timers[context] = context_timer;
	}
	context_stack.push_back(context);
}

void end_profile(std::string context) {
	if(context_stack.back() != context){
		throw std::string{"Tried to end profile context '"} + context + "' but the current context is '" + context_stack.back() + "'";
	}

	context_timers[context_stack.back()] += current_context_timer.restart();
	context_stack.pop_back();
}

void clear_profile_info() {
	context_timers.clear();
	context_stack.clear();
}

static bool compare_time(const std::pair<std::string, sf::Time>& a, const std::pair<std::string, sf::Time>& b) {
	return a.second > b.second;
}

void print_profile_info() {
	std::vector<std::pair<std::string, sf::Time>> sorted_times;
	for(auto context : context_timers){
		sorted_times.push_back(context);
	}
	std::sort(sorted_times.begin(), sorted_times.end(), compare_time);
	for(auto context : sorted_times){
		std::cout << context.first << ": " << context.second.asSeconds() << std::endl;
	}
}
