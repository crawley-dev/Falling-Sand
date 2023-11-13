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

        ImGui::Checkbox("Toggle Demo Window"        , &showDemoWindow     );         
        ImGui::Checkbox("Run Simulation Game"       , &data.runSim        );  //ImGui::SameLine();
        ImGui::Checkbox("Play Conway's Game of Life", &data.playGameOfLife);

        // doesn't work as ImGui framecount is independent of the simulation.
        //ImGui::InputInt("", &stepFrame, 1, 10); ImGui::SameLine();
        if (ImGui::Button("Step n Frames:")) stepFrame = ImGui::GetFrameCount() + 1;
        if (ImGui::Button("Reset Sim"     )) data.resetSim = true;
        if (ImGui::Button("Scan Top->Bot" )) data.scanTopDown = true; ImGui::SameLine();
        if (ImGui::Button("Scan Bot->Top" )) data.scanTopDown = false;

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
        
        if (data.playGameOfLife) {
            data.drawType = 4;
            //if (ImGui::Button("Dead Cell" )) data.drawType = 0; ImGui::SameLine();
            //if (ImGui::Button("Alive Cell")) data.drawType = 4; //ImGui::SameLine();
        } else {
            ImGui::Text("Draw Type: "    );
            if (ImGui::Button("Eraser"   )) data.drawType  = 0;   ImGui::SameLine();
            if (ImGui::Button("Sand"     )) data.drawType  = 1;   ImGui::SameLine();
            if (ImGui::Button("Water"    )) data.drawType  = 2;   ImGui::SameLine();
            if (ImGui::Button("Concrete" )) data.drawType  = 3;   //ImGui::SameLine();
        }

        ImGui::Text("Draw Shape:    ");
        if (ImGui::Button("Circlular"        )) data.drawShape = 0; ImGui::SameLine();
        if (ImGui::Button("Circlular Outline")) data.drawShape = 1; ImGui::SameLine();
        if (ImGui::Button("Line"             )) data.drawShape = 2; ImGui::SameLine();
        if (ImGui::Button("Square"           )) data.drawShape = 3;

        ImGui::InputInt("Cell Draw Size (px)" , &data.drawSize,       1, 10);
        ImGui::InputInt("Cell Draw Chance (%)", &data.drawChance,     1, 10);
        ImGui::InputInt("Cell Colour Variance", &data.drawColourVariance, 1, 10);
    }

    ImGui::SeparatorText("Debug Values"); 
    {
        bool OutofBounds = false;
        TextureData& texture = data.textures[BACKGROUND_TEXTURE_IDX];
        if (data.mouseX > texture.width || data.mouseX < 0 || data.mouseY > texture.height || data.mouseY < 0) OutofBounds = true;

        ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
        const int TITLE_BAR_OFFSET_X = 8;
        const int TITLE_BAR_OFFSET_Y = 28;
        const int COLOUR_VARIANCE_RANGE = 20;
        data.mouseX = (int)(io.MousePos.x - windowPos.x - TITLE_BAR_OFFSET_X);
        data.mouseY = (int)(io.MousePos.y - windowPos.y - TITLE_BAR_OFFSET_Y);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("Application framecount: %d\n" , ImGui::GetFrameCount());
        ImGui::Text("Scale Factor: %d\n"           , data.scaleFactor    );
        ImGui::Text("Reloaded Texture: %d Times\n" , data.texReloadCount );
        ImGui::Text("Texture Width: %d\n"          , texture.width       );
        ImGui::Text("Texture Height: %d\n"         , texture.height      );
        ImGui::Text("Mouse X: %d\n"                , data.mouseX         );
        ImGui::Text("Mouse Y: %d\n"                , data.mouseY         );
        ImGui::Text("Mouse Out of Bounds? %d\n"    , OutofBounds         );
        //for (auto tex : data.textures) 
        //ImGui::Text("textureID:%d\n"          , tex.id              );

    }

    ImGui::End();
}

// GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged
void Interface::gameWindow(interfaceData& data)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;
    TextureData& bg = data.textures[BACKGROUND_TEXTURE_IDX];
    TextureData& texture = data.textures[GAME_TEXTURE_IDX];

    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow", not win32 application
    const int textureOffsetX = 10;
    const int textureOffsetY = 20 + 10 + 10;
    if (   texture.width + textureOffsetX != (int)ImGui::GetWindowSize().x 
        || texture.height + textureOffsetY != (int)ImGui::GetWindowSize().y) 
    {
        data.reloadGame = true;
        texture.width = ImGui::GetWindowSize().x - textureOffsetX;
        texture.height = ImGui::GetWindowSize().y - textureOffsetY;
    }
    else data.reloadGame = false;

    {
        ImGui::BeginChild("GameRender");
        ImVec2 textureRenderSize = ImVec2(texture.width, texture.height); // STRETCHING: bg --> texture 
        ImGui::Image((ImTextureID)texture.id, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        //ImGui::Image((ImTextureID)bg.id, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}