#pragma once

#include "pch.h"
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
{   // pair of empty brackets {} defines a separate scope, required for each separator.
    ImGui::Begin("Debug Menu");
    

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Simulation Settings")) 
    {
        ImGui::SeparatorText("Simulation Settings");
        //ImGui::Checkbox("Toggle Demo Window"        , &showDemoWindow     );         
        ImGui::Checkbox("Run Simulation"            , &data.runSim        );
        ImGui::Checkbox("Play Conway's Game of Life", &data.playGameOfLife);
        
        if (ImGui::Button("Reset Sim"     )) data.resetSim    =  true;
        if (ImGui::Button("Scan Top->Bot" )) data.scanTopDown =  true; ImGui::SameLine();
        if (ImGui::Button("Scan Bot->Top" )) data.scanTopDown = false;

        if (ImGui::Button("Decrease Cell Scale")) {
            data.scaleFactor--;
            data.reloadGame = true;
        } ImGui::SameLine();
        if (ImGui::Button("Increase Cell Scale")) {
            data.scaleFactor++;
            data.reloadGame = true;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Texture Manipulation")) 
    {
        ImGui::SeparatorText("Manipulating Textures");

        static char str[128] = ""; // could overflow the buffer. yay.. this will probably not work at some point..
        ImGui::InputTextWithHint("Enter File Location", "../Resources/", str, IM_ARRAYSIZE(str));
        
        if (ImGui::Button("Change Texture")) loadedTex++; ImGui::SameLine();
        if (ImGui::Button("Load Image")) {
            data.loadImage = true;
            data.imagePath = "../Resources/" + std::string(str);
        }
        else data.loadImage = false;

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Frame Stepping"))
    {
        ImGui::SeparatorText("Frame Stepping");
        
        static int framesToStep = 0;
        static int pseudoFrames = 1;
        static bool doFrameStepping     = false;

        if (framesToStep > 0 && doFrameStepping) {
            data.runSim = true;
            framesToStep--;
        }
        else if (doFrameStepping) data.runSim = false;

        ImGui::PushButtonRepeat(true); // lets you hold down a button to repeat it.
        if (ImGui::ArrowButton("##left", ImGuiDir_Left  )) { pseudoFrames--; } ImGui::SameLine();
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { pseudoFrames++; } ImGui::SameLine();
        if (pseudoFrames < 0) pseudoFrames = 0;
        ImGui::Text("%d", pseudoFrames);

        if (ImGui::Button("Step Frames")) {
            framesToStep = pseudoFrames;
            doFrameStepping = true;
        } ImGui::SameLine();
        if (ImGui::Button("Disable Frame Stepping")) {
            doFrameStepping = false;
            data.runSim = true;
        }

        ImGui::PopButtonRepeat(); // Imgui configuration is implemented with a stack? interesting
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Cell Drawing"))
    {
        ImGui::SeparatorText("Cell Drawing");
        // doesn't currently highlight which type is selected.
        // dig into deeper logic of ImGui.. 
        
        if (data.playGameOfLife) {
            data.drawType = 4; // ALIVE cell
        } else {
            ImGui::Text("Draw Type: "    ); // could probably do some loop when i have like 50 materials
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
        
        ImGui::TreePop(); // might get a bit angry :0 if no tree to close 
    }

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Debug Values"))
    {
        ImGui::SeparatorText("Debug Values"); 
        bool OutofBounds = false;
        TextureData& texture = data.textures[loadedTex];
        if (data.mouseX > texture.width || data.mouseX < 0 || data.mouseY > texture.height || data.mouseY < 0) OutofBounds = true;

        ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
        const int TITLE_BAR_OFFSET_X = 8;
        const int TITLE_BAR_OFFSET_Y = 28;
        const int COLOUR_VARIANCE_RANGE = 20;
        data.mouseX = (int)(io.MousePos.x - windowPos.x - TITLE_BAR_OFFSET_X);
        data.mouseY = (int)(io.MousePos.y - windowPos.y - TITLE_BAR_OFFSET_Y);

        ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("Application Framecount: %d\n" , ImGui::GetFrameCount());
        ImGui::Text("Game Framecount: %d\n"        , data.frame          );
        ImGui::Text("Scale Factor: %d\n"           , data.scaleFactor    );
        ImGui::Text("Reloaded Texture: %d Times\n" , data.texReloadCount );
        ImGui::Text("Displayed Texture: %d\n"      , loadedTex           );
        ImGui::Text("Texture Width: %d\n"          , texture.width       );
        ImGui::Text("Texture Height: %d\n"         , texture.height      );
        ImGui::Text("Mouse X: %d\n"                , data.mouseX         );
        ImGui::Text("Mouse Y: %d\n"                , data.mouseY         );
        ImGui::Text("Mouse Out of Bounds? %d\n"    , OutofBounds         );
        ImGui::Text("Scanning Top Down? %d\n"      , data.scanTopDown    );

        ImGui::TreePop();
    }

    ImGui::End();
}

// GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged
void Interface::gameWindow(interfaceData& data)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;

    //loadedTex = loadedTex % data.textures.size(); // should be fiine
    loadedTex = loadedTex % 2;
    TextureData& texture = data.textures[loadedTex];
    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow",
    //       Not the SDL2 generated win32 window

    const int xOffset = 10;
    const int yOffset = 40;
    const int windowX = (int)ImGui::GetWindowSize().x;
    const int windowY = (int)ImGui::GetWindowSize().y;
    if (texture.width + xOffset != windowX || texture.height + yOffset != windowY) {
        data.reloadGame = true;
        texture.width   = (int)ImGui::GetWindowSize().x - xOffset;
        texture.height  = (int)ImGui::GetWindowSize().y - yOffset;
    }
    else data.reloadGame = false;

    {
        ImGui::BeginChild("GameRender");
        ImVec2 textureRenderSize = ImVec2(texture.width, texture.height); // STRETCHING: bg --> texture 
        ImGui::Image((ImTextureID)texture.id, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}