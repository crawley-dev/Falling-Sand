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

        ImGui::Checkbox("Toggle Demo Window" , &showDemoWindow);         
        ImGui::Checkbox("Run Simulation Game", &data.runSim   );  //ImGui::SameLine();

        // doesn't work as ImGui framecount is independent of the simulation.
        //ImGui::InputInt("", &stepFrame, 1, 10); ImGui::SameLine();
        if (ImGui::Button("Step n Frames:")) stepFrame = ImGui::GetFrameCount() + 1;
        if (ImGui::Button("Reset Sim"     )) data.resetSim = true;
        if (ImGui::Button("Top->Bot")) data.scanTopDown = true; ImGui::SameLine();
        if (ImGui::Button("Bot->Top")) data.scanTopDown = false;
        if (ImGui::Button("Decrease Cell Scale")) {
            data.scaleFactor--;
            data.reloadGame = true;
        } ImGui::SameLine();
        if (ImGui::Button("Increase Cell Scale")) {
            data.scaleFactor++;
            data.reloadGame = true;
        }
    }

    ImGui::SeparatorText("Cell Drawing");
    {
        // doesn't currently highlight which type is selected.
        // dig into deeper logic of ImGui.. ugh
        ImGui::Text("Draw Type: "    );
        if (ImGui::Button("Eraser"   )) data.drawType  = 0;   ImGui::SameLine();
        if (ImGui::Button("Sand"     )) data.drawType  = 1;   ImGui::SameLine();
        if (ImGui::Button("Water"    )) data.drawType  = 2;   ImGui::SameLine();
        if (ImGui::Button("Concrete" )) data.drawType  = 3;   //ImGui::SameLine();

        ImGui::Text("Draw Shape:    ");
        if (ImGui::Button("Circlular")) data.drawShape = 0; ImGui::SameLine();
        if (ImGui::Button("Line"     )) data.drawShape = 1; ImGui::SameLine();
        if (ImGui::Button("Square"   )) data.drawShape = 2;

        ImGui::InputInt("Cell Draw Size (px)" , &data.drawSize,       1, 10);
        ImGui::InputInt("Cell Draw Chance (%)", &data.drawChance,     1, 10);
        ImGui::InputInt("Cell Colour Variance", &data.drawColourVariance, 1, 10);
    }

    ImGui::SeparatorText("Debug Values"); 
    {
        bool OutofBounds = false;
        if (data.mouseX > data.textureWidth || data.mouseX < 0 || data.mouseY > data.textureHeight || data.mouseY < 0) OutofBounds = true;

        ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
        const int TITLE_BAR_OFFSET_X = 8;
        const int TITLE_BAR_OFFSET_Y = 28;
        const int COLOUR_VARIANCE_RANGE = 20;
        data.mouseX = (int)(io.MousePos.x - windowPos.x - TITLE_BAR_OFFSET_X);
        data.mouseY = (int)(io.MousePos.y - windowPos.y - TITLE_BAR_OFFSET_Y);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("Application framecount: %d\n", ImGui::GetFrameCount());
        ImGui::Text("textureID:%d\n"              , data.textureID        );
        ImGui::Text("Reloaded Texture: %d Times\n", data.textureReloadCount   );
        ImGui::Text("textureWidthidth: %d\n"          , data.textureWidth         );
        ImGui::Text("textureHeight: %d\n"         , data.textureHeight             );
        ImGui::Text("Mouse X: %d\n"               , data.mouseX        );
        ImGui::Text("Mouse Y: %d\n"               , data.mouseY        );
        ImGui::Text("Out of Bounds? %d\n"         , OutofBounds           );
    }

    ImGui::End();
}

// GLuint textureID, int& textureWidthidth, int& textureHeight, bool& hasSizeChanged
void Interface::gameWindow(interfaceData& data)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;

    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow", not win32 application
    const int textureOffsetX = 10;
    const int textureOffsetY = 20 + 10 + 10;
    if (   data.textureWidth + textureOffsetX != (int)ImGui::GetWindowSize().x
        || data.textureHeight + textureOffsetY != (int)ImGui::GetWindowSize().y) 
    {
        data.reloadGame = true;
        data.textureWidth = ImGui::GetWindowSize().x - textureOffsetX;
        data.textureHeight = ImGui::GetWindowSize().y - textureOffsetY;
    }
    else data.reloadGame = false;

    {
        ImGui::BeginChild("GameRender");
        ImVec2 textureRenderSize = ImVec2(data.textureWidth, data.textureHeight);
        ImGui::Image((ImTextureID)data.textureID, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}