//
//  winutil.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#ifndef BoE_boe_windows_h
#define BoE_boe_windows_h

#include <SFML/Window.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem; // TODO: Centralize this alias

bool isFrontWindow(sf::Window& win);
void makeFrontWindow(sf::Window& win);

void init_fileio();

fs::path nav_get_party();
fs::path nav_put_party();
fs::path nav_get_scenario();
fs::path nav_put_scenario();

class ModalSession {
	void* session;
public:
	void pumpEvents();
	ModalSession(sf::Window& win);
	~ModalSession();
};

#endif
