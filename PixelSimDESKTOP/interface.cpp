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

void Interface::debugMenu(AppState& state)
{   // pair of empty brackets {} defines a separate scope, required for each separator.
    ImGui::Begin("Debug Menu");
    

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Simulation Settings")) 
    {
        ImGui::SeparatorText("Simulation Settings");
        ImGui::Checkbox("Run Simulation", &state.runSim);
        
        if (ImGui::Button("Reset Sim")) state.resetSim = true;

        if (ImGui::Button("Decrease Cell Scale")) {
            state.scaleFactor--;
            state.reloadGame = true;
        } ImGui::SameLine();
        if (ImGui::Button("Increase Cell Scale")) {
            state.scaleFactor++;
            state.reloadGame = true;
        }
        state.scaleFactor = std::clamp(state.scaleFactor, (u8)1, (u8)10);

        ImGui::Text("Update Modes: "); ImGui::SameLine();
        if (ImGui::BeginCombo("update_modes_combo", Update::names[state.updateMode].data())) {
            for (u8 n = 0; n < Update::COUNT; n++) {
                const bool is_selected = (state.updateMode == n);
                if (ImGui::Selectable(Update::names[n].data(), is_selected))
                    state.updateMode = n;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Scan Modes: "); ImGui::SameLine();
        if (ImGui::BeginCombo("scan_modes_combo", Scan::names[state.scanMode].data())) {
            for (u8 n = 0; n < Scan::COUNT; n++) {
                const bool is_selected = (state.scanMode == n);
                if (ImGui::Selectable(Scan::names[n].data(), is_selected))
                    state.scanMode = n;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        int fluidDispersionFactor = state.fluidDispersionFactor;
        ImGui::Text("Fluid Dispersion"); ImGui::SameLine();
        ImGui::InputInt("fluid_dispersion_inputint", &fluidDispersionFactor, 1, 10);
        state.fluidDispersionFactor = fluidDispersionFactor;

        int solidDispersionFactor = state.solidDispersionFactor;
        ImGui::Text("Solid Dispersion"); ImGui::SameLine();
        ImGui::InputInt("solid_dispersion_inputint", &solidDispersionFactor, 1, 10);
        state.solidDispersionFactor = solidDispersionFactor;

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Texture Manipulation")) 
    {
        ImGui::SeparatorText("Manipulating Textures");

        static char str[128] = ""; // could overflow the buffer. yay.. this will probably not work at some point..
        ImGui::InputTextWithHint("Enter File Location", "../Resources/Pictures/", str, IM_ARRAYSIZE(str));
        
        if (ImGui::Button("Change Texture")) loadedTex++; ImGui::SameLine();
        if (ImGui::Button("Load Image")) {
            state.loadImage = true;
            state.imagePath = "../Resources/Pictures/" + std::string(str);
        }
        else state.loadImage = false;

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Frame Stepping"))
    {
        ImGui::SeparatorText("Frame Stepping");
        
        static int framesToStep = 0;
        static int pseudoFrames = 1;
        static bool doFrameStepping = false;

        if (framesToStep > 0 && doFrameStepping) {
            state.runSim = true;
            framesToStep--;
        }
        else if (doFrameStepping) state.runSim = false;

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
            state.runSim = true;
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
        
        if (state.scanMode == Scan::GAME_OF_LIFE)
            state.drawMaterial = MaterialID::GOL_ALIVE;
        else {
            if (state.drawMaterial == MaterialID::GOL_ALIVE)
                state.drawMaterial = MaterialID::SAND; // TODO: store state of previous drawMaterial, don't default to sand

            ImGui::Text("Draw Mode: "); ImGui::SameLine();
            if (ImGui::BeginCombo("draw_modes_combo", MaterialID::names[state.drawMaterial].data())) {
                for (u8 n = 0; n < MaterialID::COUNT; n++) {
                    const bool is_selected = (state.drawMaterial == n);
                    if (ImGui::Selectable(MaterialID::names[n].data(), is_selected))
                        state.drawMaterial = n;

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::Text("Draw Shape:"); ImGui::SameLine();
        if (ImGui::BeginCombo("draw_shape_combo", Shape::names[state.drawShape].data())) {
            for (u8 n = 0; n < Shape::COUNT; n++) {
                const bool is_selected = (state.drawShape == n);
                if (ImGui::Selectable(Shape::names[n].data(), is_selected))
                    state.drawShape = n;
        
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        int drawSize = state.drawSize;
        ImGui::Text("Draw Size  "); ImGui::SameLine();
        ImGui::InputInt("draw_size_inputint",  &drawSize,   1, 10);
        state.drawSize = std::clamp(state.drawSize, (u16)1, (u16)1000);
        u8 drawSizeModifier = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftShift)) ? 10 : 1;
        state.drawSize += (int)io.MouseWheel * drawSizeModifier;

		int drawChance = state.drawChance;
        ImGui::Text("Draw Chance"); ImGui::SameLine();
        ImGui::InputInt("draw_chance_inputint", &drawChance, 1, 10);
        state.drawChance = std::clamp(state.drawChance, (u8)1, (u8)100);
        // ImGui::InputInt("Cell Colour Variance", (int)state.drawColourVariance, 1, 10);
        //state.drawColourVariance = std::clamp(state.drawColourVariance, 1, 255);
        // ^^ might revive this, re-generate random variant for a cell?
        
        ImGui::TreePop(); 
    }

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Debug Values"))
    {
        ImGui::SeparatorText("Debug Values"); 
        bool OutofBounds = false;
        TextureData& texture = state.textures[loadedTex];
        if (state.mouseX > texture.width || state.mouseX < 0 || state.mouseY > texture.height || state.mouseY < 0) OutofBounds = true;

        ImVec2 windowPos = ImGui::GetMainViewport()->Pos;
        const int TITLE_BAR_OFFSET_X = 8;
        const int TITLE_BAR_OFFSET_Y = 28;
        const int COLOUR_VARIANCE_RANGE = 20;
        const char* scanMode = Scan::names[state.scanMode].data();
        state.mouseX = (int)(io.MousePos.x - windowPos.x - TITLE_BAR_OFFSET_X);
        state.mouseY = (int)(io.MousePos.y - windowPos.y - TITLE_BAR_OFFSET_Y);


        ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
        ImGui::Text("Application Framecount: %d\n" ,    ImGui::GetFrameCount());
        ImGui::Text("Game Framecount: %d\n"        ,    state.frame             );
        ImGui::Text("Scale Factor: %d\n"           ,    state.scaleFactor       );
        ImGui::Text("Textures Reloaded: %d Times\n",    state.texReloadCount    );
        ImGui::Text("Displayed Texture: %s\n"      ,    TexID::names[loadedTex].data());
        ImGui::Text("Texture Width: %d\n"          ,    texture.width           );
        ImGui::Text("Texture Height: %d\n"         ,    texture.height          );
        ImGui::Text("Cell Width: %d\n"             ,    texture.width/state.scaleFactor);
        ImGui::Text("Cell Height: %d\n"            ,    texture.height/state.scaleFactor);
        ImGui::Text("Texture Updates: %d\n"        ,    state.textureChanges    );
        ImGui::Text("Cell Updates: %d\n"           ,    state.cellChanges       );
        ImGui::Text("Mouse X: %d\n"                ,    state.mouseX            );
        ImGui::Text("Mouse Y: %d\n"                ,    state.mouseY            );
        ImGui::Text("Mouse Out of Bounds? %d\n"    ,    OutofBounds             );

        ImGui::TreePop();
    }

    ImGui::End();
}

void Interface::gameWindow(AppState& state)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;

    loadedTex = loadedTex % state.textures.size();
    TextureData& texture = state.textures[loadedTex];
    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow",
    //       Not the SDL2 generated win32 window

    constexpr int xOffset = 10;
    constexpr int yOffset = 40;
    int windowX = (int)ImGui::GetWindowSize().x; 
    int windowY = (int)ImGui::GetWindowSize().y; 
    if (windowX % 2 != 0) ++windowX; // i don't remember what this does
    if (windowY % 2 != 0) ++windowY; // i don't remember what this does

    if (texture.width + xOffset != windowX || texture.height + yOffset != windowY) {
        state.reloadGame = true;
        texture.width   = (int)ImGui::GetWindowSize().x - xOffset;
        texture.height  = (int)ImGui::GetWindowSize().y - yOffset;
        if (texture.width  % 2 != 0) ++texture.width;  // i don't remember what this does
        if (texture.height % 2 != 0) ++texture.height; // i don't remember what this does
        ImGui::SetWindowSize(ImVec2(texture.width, texture.height));
    }
    else state.reloadGame = false;

    {
        ImGui::BeginChild("GameRender");
        ImVec2 textureRenderSize = ImVec2(texture.width, texture.height);
        ImGui::Image((ImTextureID)texture.id, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}