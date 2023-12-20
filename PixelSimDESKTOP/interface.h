#pragma once
#include "pch.h"
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
    int loadedTex = 0;
    ImGuiIO& io = ImGui::GetIO();
    //TextureData texture;
};
