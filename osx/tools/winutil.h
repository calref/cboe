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
void makeFrontWindow(sf::Window& win, bool acceptKeyInput = true);

void init_fileio();

struct no_file_chosen {}; // an exception class

fs::path nav_get_party() throw(no_file_chosen);
fs::path nav_put_party() throw(no_file_chosen);
fs::path nav_get_scenario() throw(no_file_chosen);
fs::path nav_put_scenario() throw(no_file_chosen);

class ModalSession {
	void* session;
public:
	void pumpEvents();
	ModalSession(sf::Window& win);
	~ModalSession();
};

#endif
