
#ifndef FRAMERATE_LIMITER_HPP
#define FRAMERATE_LIMITER_HPP

#include <SFML/Graphics.hpp>

class cFramerateLimiter {
public:
	
	cFramerateLimiter(int desired_fps = 60);
	
	void frame_finished();
	
private:
	
	sf::Clock clock;
	const sf::Int64 desired_microseconds_per_frame; 
};

#endif
