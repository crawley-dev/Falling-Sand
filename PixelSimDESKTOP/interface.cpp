#pragma once

#include "interface.h"

Interface::Interface() {}
Interface::~Interface() {}

// pass a structure with all variables for nice viewing?? idk how else to?? template???? <-- prob not sounds complex
void Interface::main()
{
    io = ImGui::GetIO(); // keep IO upto date

    boilerPlate();
    demoWindow();
    //debugMenu(interfaceData& data);
}

void Interface::boilerPlate()
{
    ImGui_ImplOpenGL3_NewFrame();   // init openGl frame  
    ImGui_ImplSDL2_NewFrame();      // init SDL2   frame
    ImGui::NewFrame();              // init imgui  frame

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // add background dockspace by default
}

// GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount, 
// bool &runSim, int& pxDrawType, int& drawSize
void Interface::debugMenu(interfaceData& data)
{   // A Begin/End Pair (the brackets) to contain a named window's creation and destruction. required!!
    ImGui::Begin("Debug Menu");                                     // Create a window called "Hello, world!" and append into it  
    
    ImGui::SeparatorText("Bools");
    {   // Edit bool toggling on/off the demo window
        ImGui::Checkbox("Toggle Demo Window", &showDemoWindow);         
        ImGui::Checkbox("Run Simulation Game", &data.runSim);
    }

    ImGui::SeparatorText("Values"); 
    {
        ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
        const float titleBarOffsetX = 8.f;
        const float titleBarOffsetY = 28.f;
        const float mousePosX = io.MousePos.x - windowPos.x - titleBarOffsetX;
        const float mousePosY = io.MousePos.y - windowPos.y - titleBarOffsetY;

        bool inBounds = true;
        if (mousePosX > data.texW || mousePosX < 0 || mousePosY > data.texH || mousePosY < 0) inBounds = true;
        else inBounds = false;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("textureID:%d\n", data.texID);
        ImGui::Text("Reloaded Texture: %d Times\n", data.texReloadCount);
        ImGui::Text("textureWidth: %d\n", data.texW);
        ImGui::Text("textureHeight: %d\n", data.texH);
        ImGui::Text("Mouse X: %f\n", mousePosX);
        ImGui::Text("Mouse Y: %f\n", mousePosY);
        ImGui::Text("Out of Bounds? %d\n", inBounds);
    }

    ImGui::SeparatorText("Variables");
    {
        ImGui::SliderInt("CellDraw Type", &data.clDrawType, 0, 3); // change to buttons later
        ImGui::InputInt("Cell Draw Size", &data.clDrawSize, 1, 10);
        //ImGui::SliderInt("Pixel Draw Size", &drawSize, 0, 1000);

    }

    ImGui::End();
}

// GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged
void Interface::gameWindow(interfaceData& data)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;


    const int textureOffsetX = 10;
    const int textureOffsetY = 20 + 10 + 10;
    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow", not win32 application
    if (data.texW + textureOffsetX != (int)ImGui::GetWindowSize().x 
       || data.texH + textureOffsetY != (int)ImGui::GetWindowSize().y) {
        data.hasSizeChanged  = true;

        data.texW = ImGui::GetWindowSize().x - textureOffsetX;
        data.texH = ImGui::GetWindowSize().y - textureOffsetY;
    }
    else data.hasSizeChanged = false;

    {
        ImGui::BeginChild("GameRender");
        ImVec2 textureRenderSize = ImVec2(data.texW, data.texH);
        ImGui::Image((ImTextureID)data.texID, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}