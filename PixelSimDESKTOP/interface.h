#pragma once
#include "pch.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "interfaceData.h"

class Interface {
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
    int loadedTex = 0;
    ImGuiIO& io = ImGui::GetIO();
};
