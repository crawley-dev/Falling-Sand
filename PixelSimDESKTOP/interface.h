#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "interfaceData.h"

class Interface
{
public:
    Interface();
    ~Interface();

    void main();
    void boilerPlate();
    void debugMenu(interfaceData& data);
    void gameWindow(interfaceData& data);
    inline void demoWindow() { if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow); }

private:
    bool showDemoWindow = true;
    float frameRate = 0;
    ImGuiIO& io = ImGui::GetIO();
    //TextureData texture;
};
