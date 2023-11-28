#pragma once

#include "pch.h"
#include "framework.h"

Framework* app = nullptr;

int main(int, char**)
{
    const int width = 1280;
    const int height = 720;

    //const int FPS = 144;
    //const int frameDelay = 1000 / FPS;
    //int frameStart, frameTime;


    app = new Framework();
    if (!app->init("Pixel Physics Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height)) return -1;

    while (app->running())
    {
        //frameStart = SDL_GetTicks();

        app->handleEvents();
        app->update();
        app->render();

        //frameTime = SDL_GetTicks() - frameStart;
        //if (frameDelay > frameTime) { SDL_Delay(frameDelay - frameTime); }
    }

    app->clean();
    return 0;
}