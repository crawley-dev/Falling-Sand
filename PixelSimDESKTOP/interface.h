#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>

class Interface
{
public:
    Interface();
    ~Interface();

    void main(GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount, bool &runSim, int& pxDrawType);
    void boilerPlate();
    void debugMenu(GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount, bool &runSim, int& pxDrawType, bool inBounds);
    void gameWindow(GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged);
    inline void demoWindow() { if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow); }

private:
    bool showDemoWindow = true;
    float frameRate = 0;
    ImGuiIO& io = ImGui::GetIO();
};
