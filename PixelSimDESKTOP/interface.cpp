#pragma once

#include "interface.h"

Interface::Interface() {}
Interface::~Interface() {}

// pass a structure with all variables for nice viewing?? idk how else to?? template???? <-- prob not sounds complex
void Interface::main(GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount, bool &runSim, int &pxDrawType)
{
    io = ImGui::GetIO(); // keep IO upto date

    boilerPlate();
    demoWindow();
    //debugMenu(textureID, textureWidth, textureHeight, texReloadedCount, runSim, pxDrawType);
}

void Interface::boilerPlate()
{
    ImGui_ImplOpenGL3_NewFrame();   // init openGl frame  
    ImGui_ImplSDL2_NewFrame();      // init SDL2   frame
    ImGui::NewFrame();              // init imgui  frame

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // add background dockspace by default
}

void Interface::debugMenu(GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount, bool &runSim, int& pxDrawType, bool inBounds)
{   // A Begin/End Pair (the brackets) to contain a named window's creation and destruction. required!!
    ImGui::Begin("Debug Menu");                                     // Create a window called "Hello, world!" and append into it  
    
    ImGui::SeparatorText("Bools");
    {   // Edit bool toggling on/off the demo window
        ImGui::Checkbox("Toggle Demo Window", &showDemoWindow);         
        ImGui::Checkbox("Run Simulation Game", &runSim);
    }

    ImGui::SeparatorText("Values"); 
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("textureID:%d\n", textureID);
        ImGui::Text("Reloaded Texture: %d Times\n", texReloadedCount);
        ImGui::Text("textureWidth: %d\n", textureWidth);
        ImGui::Text("textureHeight: %d\n", textureHeight);

        ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
        const float titleBarOffsetX = 8.f;
        const float titleBarOffsetY = 28.f;
        const float trueXPos = io.MousePos.x - windowPos.x - titleBarOffsetX;
        const float trueYPos = io.MousePos.y - windowPos.y - titleBarOffsetY;

        ImGui::Text("Mouse X: %f\n", trueXPos);
        ImGui::Text("Mouse Y: %f\n", trueYPos);
        ImGui::Text("Out of Bounds? %d\n", inBounds);
    }

    ImGui::SeparatorText("Variables");
    {
        ImGui::SliderInt("Pixel Draw Type", &pxDrawType, 0, 3); // change to buttons later
    }

    ImGui::End();
}

void Interface::gameWindow(GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;

    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow", not win32 application
    if (textureWidth != (int)ImGui::GetWindowSize().x || textureHeight != (int)ImGui::GetWindowSize().y) {
        hasSizeChanged  = true;
        textureWidth    = ImGui::GetWindowSize().x;
        textureHeight   = ImGui::GetWindowSize().y;
    }
    else hasSizeChanged = false;

    {
        ImGui::BeginChild("GameRender");
        ImGui::Image((ImTextureID)textureID, ImVec2(textureWidth, textureHeight), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}