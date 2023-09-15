#pragma once

#include "interface.h"

Interface::Interface() {}
Interface::~Interface() {}

void Interface::main()
{
    io = ImGui::GetIO(); // keep IO upto date

    boilerPlate();
    demoWindow();
}

void Interface::boilerPlate()
{
    ImGui_ImplOpenGL3_NewFrame();                           // init openGl frame  
    ImGui_ImplSDL2_NewFrame();                              // init SDL2   frame
    ImGui::NewFrame();                                      // init imgui  frame
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // adds docking by default
}

void Interface::debugMenu(interfaceData& data)
{   // A Begin/End Pair (the brackets) to contain a named window's creation and destruction. required!!
    ImGui::Begin("Debug Menu");

    ImGui::SeparatorText("Bools");
    {   // Edit bool toggling on/off the demo window
        static int stepFrame = -1;
        if (stepFrame == ImGui::GetFrameCount()) data.runSim = true;
        else if (stepFrame + 1 == ImGui::GetFrameCount()) data.runSim = false;

        ImGui::Checkbox("Toggle Demo Window", &showDemoWindow);         
        ImGui::Checkbox("Run Simulation Game", &data.runSim); // ImGui::SameLine();
        if (ImGui::Button("Step 1 Frame.")) stepFrame = ImGui::GetFrameCount() + 1;
        if (ImGui::Button("Reset Sim")) data.resetSim = true;

        if (ImGui::Button("Eraser")) data.clDrawType = 0; ImGui::SameLine();
        if (ImGui::Button("Sand")) data.clDrawType = 1; ImGui::SameLine();
        if (ImGui::Button("Water")) data.clDrawType = 2; ImGui::SameLine();
        if (ImGui::Button("Concrete")) data.clDrawType = 3; //ImGui::SameLine();
    }

    ImGui::SeparatorText("Cell Drawing");
    {
        //ImGui::SliderInt("CellDraw Type", &data.clDrawType, 0, 3);
        ImGui::InputInt("Cell Draw Size (px)", &data.clDrawSize, 1, 10);
        ImGui::InputInt("Cell Draw Chance (%)", &data.clDrawChance, 1, 10);
        ImGui::InputInt("Cell Colour Variance", &data.clColourVariance, 1, 10);
    }

    ImGui::SeparatorText("Debug Values"); 
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
        ImGui::Text("Application framecount: %d\n", ImGui::GetFrameCount());
        ImGui::Text("textureID:%d\n", data.texID);
        ImGui::Text("Reloaded Texture: %d Times\n", data.texReloadCount);
        ImGui::Text("textureWidth: %d\n", data.texW);
        ImGui::Text("textureHeight: %d\n", data.texH);
        ImGui::Text("Mouse X: %f\n", mousePosX);
        ImGui::Text("Mouse Y: %f\n", mousePosY);
        ImGui::Text("Out of Bounds? %d\n", inBounds);
    }


    ImGui::End();
}

// GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged
void Interface::gameWindow(interfaceData& data)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;

    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow", not win32 application
    const int textureOffsetX = 10;
    const int textureOffsetY = 20 + 10 + 10;
    if (data.texW + textureOffsetX != (int)ImGui::GetWindowSize().x 
       || data.texH + textureOffsetY != (int)ImGui::GetWindowSize().y) 
    {
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