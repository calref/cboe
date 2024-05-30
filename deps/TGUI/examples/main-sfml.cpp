/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Core.hpp>
#include <TGUI/Backends/SFML.hpp>

bool runExample(tgui::GuiBase& gui);

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "TGUI example");
    window.setFramerateLimit(60);

    tgui::Gui gui(window);
    if (!runExample(gui))
        return EXIT_FAILURE;

    gui.mainLoop();
    return EXIT_SUCCESS;
}
