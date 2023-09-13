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

void Interface::debugMenu(GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount, bool &runSim, int& pxDrawType, int& drawSize)
{   // A Begin/End Pair (the brackets) to contain a named window's creation and destruction. required!!
    ImGui::Begin("Debug Menu");                                     // Create a window called "Hello, world!" and append into it  
    
    ImGui::SeparatorText("Bools");
    {   // Edit bool toggling on/off the demo window
        ImGui::Checkbox("Toggle Demo Window", &showDemoWindow);         
        ImGui::Checkbox("Run Simulation Game", &runSim);
    }

    ImGui::SeparatorText("Values"); 
    {
        ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
        const float titleBarOffsetX = 8.f;
        const float titleBarOffsetY = 28.f;
        const float mousePosX = io.MousePos.x - windowPos.x - titleBarOffsetX;
        const float mousePosY = io.MousePos.y - windowPos.y - titleBarOffsetY;

        bool inBounds = true;
        if (mousePosX > textureWidth || mousePosX < 0 || mousePosY > textureHeight || mousePosY < 0) inBounds = true;
        else inBounds = false;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("textureID:%d\n", textureID);
        ImGui::Text("Reloaded Texture: %d Times\n", texReloadedCount);
        ImGui::Text("textureWidth: %d\n", textureWidth);
        ImGui::Text("textureHeight: %d\n", textureHeight);
        ImGui::Text("Mouse X: %f\n", mousePosX);
        ImGui::Text("Mouse Y: %f\n", mousePosY);
        ImGui::Text("Out of Bounds? %d\n", inBounds);
    }

    ImGui::SeparatorText("Variables");
    {
        ImGui::SliderInt("Pixel Draw Type", &pxDrawType, 0, 3); // change to buttons later
        ImGui::InputInt("Pixel Draw Size", &drawSize, 1, 10);
        //ImGui::SliderInt("Pixel Draw Size", &drawSize, 0, 1000);

    }

    ImGui::End();
}

void Interface::gameWindow(GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;


    const int textureOffsetX = 10;
    const int textureOffsetY = 20 + 10 + 10;
    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow", not win32 application
    if (textureWidth + textureOffsetX != (int)ImGui::GetWindowSize().x 
       || textureHeight + textureOffsetY != (int)ImGui::GetWindowSize().y) {
        hasSizeChanged  = true;

        textureWidth = ImGui::GetWindowSize().x - textureOffsetX;
        textureHeight = ImGui::GetWindowSize().y - textureOffsetY;
    }
    else hasSizeChanged = false;

    {
        ImGui::BeginChild("GameRender");
        ImGui::Image((ImTextureID)textureID, ImVec2(textureWidth, textureHeight), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}