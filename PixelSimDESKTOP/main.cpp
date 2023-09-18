#pragma once
// extreme homosexuality L 
#include "framework.h"

Framework* app = nullptr;

int main(int, char**)
{
    const int width = 1280;
    const int height = 720;

    app = new Framework();
    if (!app->init("Pixel Physics Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height)) return -1;

    while (app->running())
    {
        app->handleEvents();
        app->update();
        app->render();
    }

    app->clean();
    return 0;
}