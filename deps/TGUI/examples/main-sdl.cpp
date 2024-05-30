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
#include <TGUI/Backends/SDL.hpp>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_opengl.h>

bool runExample(tgui::GuiBase& gui);

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying SDL
void run_application(SDL_Window* window)
{
    tgui::GuiSDL gui{window};
    if (!runExample(gui))
        return;

    gui.mainLoop();
}

// Note that no error checking is performed on SDL initialization in this example code
int main(int, char **)
{
    SDL_Init(SDL_INIT_VIDEO);

    // The SDL backend in TGUI currently requires at least OpenGL 4.1 (or OpenGL ES 3.0 on Android)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // TGUI requires a window created with the SDL_WINDOW_OPENGL flag and an OpenGL context
    SDL_Window* window = SDL_CreateWindow("TGUI window with SDL",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glClearColor(0.8f, 0.8f, 0.8f, 1.f);

    // SDL_TTF needs to be initialized before using TGUI
    TTF_Init();

    run_application(window);

    // All TGUI resources must be destroyed before SDL_TTF is cleaned up
    TTF_Quit();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
