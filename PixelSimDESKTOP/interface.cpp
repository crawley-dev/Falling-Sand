#pragma once
#include "pch.h"
#include "interface.h"
#include "cell.h"

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
        ImGui::Checkbox("Run Simulation"            , &data.runSim);
        
        if (ImGui::Button("Reset Sim")) data.resetSim = true;

        if (ImGui::Button("Decrease Cell Scale")) {
            data.scaleFactor--;
            data.reloadGame = true;
        } ImGui::SameLine();
        if (ImGui::Button("Increase Cell Scale")) {
            data.scaleFactor++;
            data.reloadGame = true;
        }

        ImGui::Text("Update Modes: "); ImGui::SameLine();
        if (ImGui::BeginCombo(".", Update::names[data.updateMode].c_str())) {
            for (u8 n = 0; n < Update::names.size(); n++) {
                const bool is_selected = (data.updateMode == n);
                if (ImGui::Selectable(Update::names[n].c_str(), is_selected))
                    data.updateMode = n;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Scan Types: "); ImGui::SameLine();
        if (ImGui::BeginCombo("..", Scan::names[data.scanMode].c_str())) {
            for (u8 n = 0; n < Scan::names.size(); n++) {
                const bool is_selected = (data.scanMode == n);
                if (ImGui::Selectable(Scan::names[n].c_str(), is_selected))
                    data.scanMode = n;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        int dispersionFactor = data.dispersionFactor;
        ImGui::Text("Fluid Dispersion Rate"); ImGui::SameLine();
        ImGui::InputInt("", &dispersionFactor, 1, 10);
        data.dispersionFactor = dispersionFactor;

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Texture Manipulation")) 
    {
        ImGui::SeparatorText("Manipulating Textures");

        static char str[128] = ""; // could overflow the buffer. yay.. this will probably not work at some point..
        ImGui::InputTextWithHint("Enter File Location", "../Resources/Pictures/", str, IM_ARRAYSIZE(str));
        
        if (ImGui::Button("Change Texture")) loadedTex++; ImGui::SameLine();
        if (ImGui::Button("Load Image")) {
            data.loadImage = true;
            data.imagePath = "../Resources/Pictures/" + std::string(str);
        }
        else data.loadImage = false;

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Frame Stepping"))
    {
        ImGui::SeparatorText("Frame Stepping");
        
        static int framesToStep = 0;
        static int pseudoFrames = 1;
        static bool doFrameStepping = false;

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
        // dig into deeper logic of ImGui.. bit painful..
        
        if (data.scanMode == Scan::GAME_OF_LIFE)
            data.drawMaterial = MaterialID::GOL_ALIVE;
        else {
            if (data.drawMaterial == MaterialID::GOL_ALIVE)
                data.drawMaterial = MaterialID::SAND; // TODO: store state of previous drawMaterial, don't default to sand

            ImGui::Text("Draw Type: "); ImGui::SameLine();
            if (ImGui::BeginCombo("...", MaterialID::names[data.drawMaterial].c_str())) {
                for (u8 n = 0; n < MaterialID::names.size(); n++) {
                    const bool is_selected = (data.drawMaterial == n);
                    if (ImGui::Selectable(MaterialID::names[n].c_str(), is_selected))
                        data.drawMaterial = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::Text("Draw Shape:"); ImGui::SameLine();
        if (ImGui::BeginCombo("....", Shape::names[0].c_str())) {
            for (u8 n = 0; n < Shape::names.size(); n++) {
                const bool is_selected = (data.drawShape == n);
                if (ImGui::Selectable(Shape::names[n].c_str(), is_selected))
                    data.drawShape = n;
        
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        int drawSize = data.drawSize;
        ImGui::Text("Draw Size  "); ImGui::SameLine();
        ImGui::InputInt("",  &drawSize,   1, 10);
        data.drawSize = drawSize;

		int drawChance = data.drawChance;
        ImGui::Text("Draw Chance"); ImGui::SameLine();
        ImGui::InputInt("", &drawChance, 1, 10);
        data.drawChance = drawChance;
        // ImGui::InputInt("Cell Colour Variance", (int)data.drawColourVariance, 1, 10);
        // ^^ might revive this, re-generate random variant for a cell?
        
        ImGui::TreePop(); 
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
        const char* scanMode = Scan::names[data.scanMode].c_str();
        data.mouseX = (int)(io.MousePos.x - windowPos.x - TITLE_BAR_OFFSET_X);
        data.mouseY = (int)(io.MousePos.y - windowPos.y - TITLE_BAR_OFFSET_Y);


        ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("Application Framecount: %d\n", ImGui::GetFrameCount());
        ImGui::Text("Game Framecount: %d\n"       , data.frame            );
        ImGui::Text("Scale Factor: %d\n"          , data.scaleFactor      );
        ImGui::Text("Reloaded Texture: %d Times\n", data.texReloadCount   );
        ImGui::Text("Displayed Texture: %d\n"     , loadedTex             );
        ImGui::Text("Texture Width: %d\n"         , texture.width         );
        ImGui::Text("Texture Height: %d\n"        , texture.height        );
        ImGui::Text("Mouse X: %d\n"               , data.mouseX           );
        ImGui::Text("Mouse Y: %d\n"               , data.mouseY           );
        ImGui::Text("Mouse Out of Bounds? %d\n"   , OutofBounds           );

        ImGui::TreePop();
    }

    ImGui::End();
}

void Interface::gameWindow(interfaceData& data)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;

    loadedTex = loadedTex % 2;
    TextureData& texture = data.textures[loadedTex];
    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow",
    //       Not the SDL2 generated win32 window

    constexpr int xOffset = 10;
    constexpr int yOffset = 40;
    int windowX = (int)ImGui::GetWindowSize().x; 
    int windowY = (int)ImGui::GetWindowSize().y; 
    if (windowX % 2 != 0) ++windowX;
    if (windowY % 2 != 0) ++windowY;

    if (texture.width + xOffset != windowX || texture.height + yOffset != windowY) {
        data.reloadGame = true;
        texture.width   = (int)ImGui::GetWindowSize().x - xOffset;
        texture.height  = (int)ImGui::GetWindowSize().y - yOffset;
        if (texture.width  % 2 != 0) ++texture.width;
        if (texture.height % 2 != 0) ++texture.height;
        ImGui::SetWindowSize(ImVec2(texture.width, texture.height));
    }
    else data.reloadGame = false;

    {
        ImGui::BeginChild("GameRender");
        ImVec2 textureRenderSize = ImVec2(texture.width, texture.height);
        ImGui::Image((ImTextureID)texture.id, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}