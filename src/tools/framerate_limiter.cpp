
#include "framerate_limiter.hpp"

cFramerateLimiter::cFramerateLimiter(int desired_fps)
	: desired_microseconds_per_frame { 1000000 / desired_fps } {

}

cFramerateLimiter::cFramerateLimiter(double desired_fps)
: desired_microseconds_per_frame { static_cast<int>(1000000 / desired_fps) } {
	
}

void cFramerateLimiter::frame_finished() {
	const sf::Int64 remaining_time_budget = this->desired_microseconds_per_frame - this->clock.getElapsedTime().asMicroseconds();
	if(remaining_time_budget > 0) sf::sleep(sf::microseconds(remaining_time_budget));
	this->clock.restart();
}
