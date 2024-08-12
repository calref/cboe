
#ifndef FRAMERATE_LIMITER_HPP
#define FRAMERATE_LIMITER_HPP

#include <SFML/Graphics.hpp>

class cFramerateLimiter {
public:
	static const int DEFAULT_FPS = 60;
	cFramerateLimiter(int desired_fps = DEFAULT_FPS);
	cFramerateLimiter(double desired_fps);
	
	void frame_finished();
	
private:
	
	sf::Clock clock;
	const sf::Int64 desired_microseconds_per_frame; 
};

#endif
