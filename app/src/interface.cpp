//#pragma once
//#include "interface.h"
//#include <algorithm>
//
//Interface::Interface() {
//}
//Interface::~Interface() {
//}
//
//void Interface::main() {
//    io = ImGui::GetIO(); // keep IO upto date
//
//    boilerPlate();
//    demoWindow();
//}
//
//void Interface::boilerPlate() {
//    ImGui_ImplOpenGL3_NewFrame();                           // init openGl frame
//    ImGui_ImplSDL2_NewFrame();                              // init SDL2   frame
//    ImGui::NewFrame();                                      // init imgui  frame
//    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // adds docking by default
//}
//
//void Interface::debugMenu(AppState& state) { // pair of empty brackets {} defines a separate scope, required for each separator.
//    ImGui::Begin("Debug Menu");
//
//    ImGui::SetNextItemOpen(true);
//    if (ImGui::TreeNode("Simulation Settings")) {
//        ImGui::SeparatorText("Simulation Settings");
//        ImGui::Checkbox("Run Simulation", &state.runSim);
//
//        if (ImGui::Button("Reset Sim")) state.resetSim = true;
//        ImGui::SameLine();
//        if (ImGui::Button("Reset Camera")) {
//            state.camera.x = 0;
//            state.camera.y = 0;
//        }
//
//        if (ImGui::Button("Decrease Cell Scale")) {
//            state.scaleFactor--;
//            state.reloadGame = true;
//        }
//        ImGui::SameLine();
//        if (ImGui::Button("Increase Cell Scale")) {
//            state.scaleFactor++;
//            state.reloadGame = true;
//        }
//        state.scaleFactor = std::clamp(state.scaleFactor, (u8)1, (u8)10);
//
//        ImGui::Text("Update Modes: ");
//        ImGui::SameLine();
//        if (ImGui::BeginCombo("update_modes_combo", Update::names[state.updateMode].data())) {
//            for (u8 n = 0; n < Update::COUNT; n++) {
//                const bool is_selected = (state.updateMode == n);
//                if (ImGui::Selectable(Update::names[n].data(), is_selected)) state.updateMode = n;
//
//                if (is_selected) ImGui::SetItemDefaultFocus();
//            }
//            ImGui::EndCombo();
//        }
//
//        ImGui::Text("Scan Modes: ");
//        ImGui::SameLine();
//        if (ImGui::BeginCombo("scan_modes_combo", Scan::names[state.scanMode].data())) {
//            for (u8 n = 0; n < Scan::COUNT; n++) {
//                const bool is_selected = (state.scanMode == n);
//                if (ImGui::Selectable(Scan::names[n].data(), is_selected)) state.scanMode = n;
//
//                if (is_selected) ImGui::SetItemDefaultFocus();
//            }
//            ImGui::EndCombo();
//        }
//
//        int fluidDispersionFactor = state.fluidDispersionFactor;
//        ImGui::Text("Fluid Dispersion");
//        ImGui::SameLine();
//        ImGui::InputInt("fluid_dispersion_inputint", &fluidDispersionFactor, 1, 10);
//        state.fluidDispersionFactor = fluidDispersionFactor;
//
//        int solidDispersionFactor = state.solidDispersionFactor;
//        ImGui::Text("Solid Dispersion");
//        ImGui::SameLine();
//        ImGui::InputInt("solid_dispersion_inputint", &solidDispersionFactor, 1, 10);
//        state.solidDispersionFactor = solidDispersionFactor;
//
//        ImGui::TreePop();
//    }
//
//    if (ImGui::TreeNode("Texture Manipulation")) {
//        ImGui::SeparatorText("Manipulating Textures");
//
//        static char str[128] = ""; // could overflow the buffer. yay.. this will probably not work at some point..
//        ImGui::InputTextWithHint("Enter File Location", "../Resources/Pictures/", str, IM_ARRAYSIZE(str));
//
//        if (ImGui::Button("Change Texture")) loadedTex++;
//        ImGui::SameLine();
//        if (ImGui::Button("Load Image")) {
//            state.loadImage = true;
//            state.imagePath = "../Resources/Pictures/" + std::string(str);
//        } else state.loadImage = false;
//
//        ImGui::TreePop();
//    }
//
//    if (ImGui::TreeNode("Frame Stepping")) {
//        ImGui::SeparatorText("Frame Stepping");
//
//        static int  framesToStep    = 0;
//        static int  pseudoFrames    = 1;
//        static bool doFrameStepping = false;
//
//        if (framesToStep > 0 && doFrameStepping) {
//            state.runSim = true;
//            framesToStep--;
//        } else if (doFrameStepping) state.runSim = false;
//
//        ImGui::PushButtonRepeat(true); // lets you hold down a button to repeat it.
//        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
//            pseudoFrames--;
//        }
//        ImGui::SameLine();
//        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
//            pseudoFrames++;
//        }
//        ImGui::SameLine();
//        if (pseudoFrames < 0) pseudoFrames = 0;
//        ImGui::Text("%d", pseudoFrames);
//
//        if (ImGui::Button("Step Frames")) {
//            framesToStep    = pseudoFrames;
//            doFrameStepping = true;
//        }
//        ImGui::SameLine();
//        if (ImGui::Button("Disable Frame Stepping")) {
//            doFrameStepping = false;
//            state.runSim    = true;
//        }
//
//        ImGui::PopButtonRepeat(); // Imgui configuration is implemented with a stack? interesting
//        ImGui::TreePop();
//    }
//
//    ImGui::SetNextItemOpen(true);
//    if (ImGui::TreeNode("Cell Drawing")) {
//        ImGui::SeparatorText("Cell Drawing");
//        // doesn't currently highlight which type is selected.
//        // dig into deeper logic of ImGui.. bit painful..
//
//        if (state.scanMode == Scan::GAME_OF_LIFE) state.drawMaterial = MaterialID::GOL_ALIVE;
//        else {
//            if (state.drawMaterial == MaterialID::GOL_ALIVE) state.drawMaterial = MaterialID::SAND; // TODO: store state of previous drawMaterial, don't default to sand
//
//            ImGui::Text("Draw Mode: ");
//            ImGui::SameLine();
//            if (ImGui::BeginCombo("draw_modes_combo", MaterialID::names[state.drawMaterial].data())) {
//                for (u8 n = 0; n < MaterialID::COUNT; n++) {
//                    const bool is_selected = (state.drawMaterial == n);
//                    if (ImGui::Selectable(MaterialID::names[n].data(), is_selected)) state.drawMaterial = n;
//
//                    if (is_selected) ImGui::SetItemDefaultFocus();
//                }
//                ImGui::EndCombo();
//            }
//        }
//
//        ImGui::Text("Draw Shape:");
//        ImGui::SameLine();
//        if (ImGui::BeginCombo("draw_shape_combo", Shape::names[state.drawShape].data())) {
//            for (u8 n = 0; n < Shape::COUNT; n++) {
//                const bool is_selected = (state.drawShape == n);
//                if (ImGui::Selectable(Shape::names[n].data(), is_selected)) state.drawShape = n;
//
//                if (is_selected) ImGui::SetItemDefaultFocus();
//            }
//            ImGui::EndCombo();
//        }
//
//
//        int drawSize = state.drawSize;
//        ImGui::Text("Draw Size  ");
//        ImGui::SameLine();
//        ImGui::InputInt("draw_size_inputint", &drawSize, 1, 10);
//        state.drawSize      = std::clamp(drawSize, 1, 1000);
//        u8 drawSizeModifier = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftShift)) ? 10 : 1;
//        state.drawSize += (int)io.MouseWheel * drawSizeModifier;
//
//        int drawChance = state.drawChance;
//        ImGui::Text("Draw Chance");
//        ImGui::SameLine();
//        ImGui::InputInt("draw_chance_inputint", &drawChance, 1, 10);
//        state.drawChance = std::clamp(drawChance, 1, 100);
//        // ImGui::InputInt("Cell Colour Variance", (int)state.drawColourVariance, 1, 10);
//        //state.drawColourVariance = std::clamp(state.drawColourVariance, 1, 255);
//        // ^^ might revive this, re-generate random variant for a cell?
//
//        ImGui::TreePop();
//    }
//
//    ImGui::SetNextItemOpen(true);
//    if (ImGui::TreeNode("Debug Values")) {
//        ImGui::SeparatorText("Debug Values");
//        bool         OutofBounds = false;
//        TextureData& texture     = state.textures[loadedTex];
//        if (state.mouse.x > texture.width || state.mouse.x < 0 || state.mouse.y > texture.height || state.mouse.y < 0) OutofBounds = true;
//
//        ImVec2      windowPos             = ImGui::GetMainViewport()->Pos;
//        const int   TITLE_BAR_OFFSET_X    = 8;
//        const int   TITLE_BAR_OFFSET_Y    = 28;
//        const int   COLOUR_VARIANCE_RANGE = 20;
//        const char* scanMode              = Scan::names[state.scanMode].data();
//        const u16   cellWidth             = texture.width / state.scaleFactor;
//        const u16   cellHeight            = texture.height / state.scaleFactor;
//        state.mouse.x                     = (int)(io.MousePos.x - windowPos.x - TITLE_BAR_OFFSET_X);
//        state.mouse.y                     = (int)(io.MousePos.y - windowPos.y - TITLE_BAR_OFFSET_Y);
//
//        // clang-format off
//        ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)"  ,   1000.0f / io.Framerate, io.Framerate);
//        ImGui::Text("Application Framecount: %d"                    ,   ImGui::GetFrameCount()              );
//        ImGui::Text("Game Framecount: %d"                           ,   state.frame                         );
//        ImGui::Text("Textures Reloaded: %d Times"                   ,   state.texReloadCount                );
//        ImGui::Text("Displayed Texture: %s"                         ,   TexID::names[loadedTex].data()      );
//        ImGui::Text("Texture Updates: %d"                           ,   state.textureChanges                );
//        ImGui::Text("Cell Updates: %d"                              ,   state.cellChanges                   );
//        ImGui::Text("Scale Factor: %d"                              ,   state.scaleFactor                   );
//        ImGui::Text("Texture Dimensions: (%d,%d)"                   ,   texture.width, texture.height       );
//        ImGui::Text("Simulation Dimensions: (%d,%d)"                ,   cellWidth, cellHeight               );
//        ImGui::Text("Camera Pos: (%d,%d)"                           ,   state.camera.x, state.camera.y      );
//        ImGui::Text("Mouse Pos: (%d,%d)"                            ,   state.mouse.x, state.mouse.y        );
//        ImGui::Text("Is Mouse Out of Bounds? %d"                    ,   OutofBounds                         );
//        // clang-format on
//
//        ImGui::TreePop();
//    }
//
//    ImGui::End();
//}
//
//void Interface::gameWindow(AppState& state) {
//    ImGui::Begin("GameWindow");
//
//    loadedTex            = loadedTex % state.textures.size();
//    TextureData& texture = state.textures[TexIndex::GAME];
//
//    // has to happen here to get proper ImGui::GetWindowSize() response
//    constexpr int xOffset = 14;
//    constexpr int yOffset = 40;
//    if (texture.width == 0 || texture.height == 0) {
//        printf("ZERO SIZE! frame: %d", ImGui::GetFrameCount());
//    }
//    int           windowX = (int)ImGui::GetWindowSize().x;
//    int           windowY = (int)ImGui::GetWindowSize().y;
//    if (windowX % 2 != 0) ++windowX;
//    if (windowY % 2 != 0) ++windowY;
//
//    if (texture.width + xOffset != windowX || texture.height + yOffset != windowY) {
//        state.reloadGame = true;
//        texture.width    = (int)ImGui::GetWindowSize().x - xOffset;
//        texture.height   = (int)ImGui::GetWindowSize().y - yOffset;
//        if (texture.width % 2 != 0) texture.width++;
//        if (texture.height % 2 != 0) texture.height++;
//        ImGui::SetWindowSize(ImVec2(texture.width, texture.height));
//    } else {
//        state.reloadGame = false;
//    }
//
//    {
//        ImGui::BeginChild("GameRender");
//        ImVec2 textureRenderSize = ImVec2(texture.width, texture.height);
//        ImGui::Image((ImTextureID)texture.id, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
//        if (ImGui::BeginItemTooltip()) {
//            ImGui::Text("camera:   (%d,%d)", state.camera.x, state.camera.y);
//            ImGui::Text("mouse:    (%d,%d)", state.print_mouse.x, state.print_mouse.y);
//            //ImGui::Text("viewport: (%d,%d)", state.print_viewport.x, state.print_viewport.y);
//            ImGui::Text("world:    (%d,%d)", state.print_world.x, state.print_world.y);
//            ImGui::Text("chunk:    (%d,%d)", state.print_chunk.x, state.print_chunk.y);
//            ImGui::Text("cell:     (%d,%d)", state.print_cell.x, state.print_cell.y);
//            ImGui::EndTooltip();
//        }
//
//        ImGui::EndChild();
//    }
//    ImGui::End();
//}

#pragma once
#include "interface.h"
#include <algorithm>

Interface::Interface() {
}
Interface::~Interface() {
}

void Interface::main() {
    io = ImGui::GetIO(); // keep IO upto date
    boilerPlate();
#if !DIST_MODE
    demoWindow();
#endif
}

void Interface::boilerPlate() {
    ImGui_ImplOpenGL3_NewFrame();                           // init openGl frame
    ImGui_ImplSDL2_NewFrame();                              // init SDL2   frame
    ImGui::NewFrame();                                      // init imgui  frame
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // adds docking by default
}

void Interface::debugMenu(AppState& state) { // pair of empty brackets {} defines a separate scope, required for each separator.
    ImGui::Begin("Menu");

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Simulation Settings")) {
        ImGui::SeparatorText("Simulation Settings");
        ImGui::Checkbox("Run Simulation", &state.runSim);

        if (ImGui::Button("Reset Sim")) state.resetSim = true;

        if (ImGui::Button("Decrease Game Area")) {
            state.scaleFactor++;
            state.reloadGame = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Increase Game Area")) {
            state.scaleFactor--;
            state.reloadGame = true;
        }
        state.scaleFactor = std::clamp(state.scaleFactor, (u8)1, (u8)10);

        //ImGui::Text("Scan Modes:     ");
        //ImGui::SameLine();
        //if (ImGui::BeginCombo("scan_modes_combo", Scan::names[state.scanMode].data())) {
        //    for (u8 n = 0; n < Scan::COUNT; n++) {
        //        const bool is_selected = (state.scanMode == n);
        //        if (ImGui::Selectable(Scan::names[n].data(), is_selected)) state.scanMode = n;

        //        if (is_selected) ImGui::SetItemDefaultFocus();
        //    }
        //    ImGui::EndCombo();
        //}

        int solidDispersionFactor = state.solidDispersionFactor;
        ImGui::Text("Solid Dispersion");
        ImGui::SameLine();
        ImGui::InputInt("##solid_dispersion_inputint", &solidDispersionFactor, 1, 10);
        state.solidDispersionFactor = solidDispersionFactor;

        int fluidDispersionFactor = state.fluidDispersionFactor;
        ImGui::Text("Fluid Dispersion");
        ImGui::SameLine();
        ImGui::InputInt("##fluid_dispersion_inputint", &fluidDispersionFactor, 1, 10);
        state.fluidDispersionFactor = fluidDispersionFactor;

        int gasDispersionFactor = state.gasDispersionFactor;
        ImGui::Text("Gas Dispersion  ");
        ImGui::SameLine();
        ImGui::InputInt("##gas_dispersion_inputint", &gasDispersionFactor, 1, 10);
        state.gasDispersionFactor = gasDispersionFactor;

        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Cell Drawing")) {
        ImGui::SeparatorText("Cell Drawing");
        // doesn't currently highlight which type is selected.
        // dig into deeper logic of ImGui.. bit painful..

        if (state.scanMode == Scan::GAME_OF_LIFE) {
            state.drawMaterial = MaterialID::GOL_ALIVE;
        } else {
            if (state.drawMaterial == MaterialID::GOL_ALIVE) state.drawMaterial = MaterialID::SAND; // TODO: store state of previous drawMaterial, don't default to sand

            ImGui::Text("Draw Mode: ");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##draw_modes_combo", MaterialID::names[state.drawMaterial].data())) {
                for (u8 n = 0; n < MaterialID::COUNT; n++) {
                    const bool is_selected = (state.drawMaterial == n);
                    if (ImGui::Selectable(MaterialID::names[n].data(), is_selected)) state.drawMaterial = n;

                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::Text("Draw Shape:");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##draw_shape_combo", Shape::names[state.drawShape].data())) {
            for (u8 n = 0; n < Shape::COUNT; n++) {
                const bool is_selected = (state.drawShape == n);
                if (ImGui::Selectable(Shape::names[n].data(), is_selected)) state.drawShape = n;

                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        int drawSize = state.drawSize;
        ImGui::Text("Draw Size: ");
        ImGui::SameLine();
        ImGui::InputInt("##draw_size_inputint", &drawSize, 1, 10);
        u8 drawSizeModifier = ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftShift)) ? 10 : 1;
        drawSize += (int)io.MouseWheel * drawSizeModifier;
        state.drawSize = std::clamp(drawSize, 1, 100);
        //state.drawSize = (state.drawSize > 10000) ? 1 : state.drawSize;

        int drawChance = state.drawChance;
        ImGui::Text("Draw Chance");
        ImGui::SameLine();
        ImGui::InputInt("##draw_chance_inputint", &drawChance, 1, 10);
        state.drawChance = std::clamp(drawChance, 1, 100);

        ImGui::TreePop();
    }

    //if (ImGui::TreeNode("Game Manipulation")) {

    //    ImGui::SeparatorText("Load an image from a file");
    //    static char imgStr[128] = ""; // could overflow the buffer. yay.. this will probably not work at some point..
    //    ImGui::InputTextWithHint("##load_image_input_text", "example.png", imgStr, IM_ARRAYSIZE(imgStr)); //TODO: default file extension
    //    if (ImGui::Button("Change Texture")) loadedTex++;
    //    ImGui::SameLine();
    //    if (ImGui::Button("Load Image")) {
    //        state.loadImage = true;
    //        state.imagePath = std::string(imgStr);
    //    } else state.loadImage = false;


    //    ImGui::SeparatorText("Save/Load game state");
    //    static char saveLoadStr[128] = "";
    //    ImGui::InputTextWithHint("##save_file_input_text ", "example", saveLoadStr, IM_ARRAYSIZE(saveLoadStr));
    //    if (ImGui::Button("Save Simulation State")) {
    //        state.saveSim  = true;
    //        state.savePath = std::string(saveLoadStr);
    //    } else state.saveSim = false;

    //    ImGui::SameLine();
    //    if (ImGui::Button("Load Simulation  State")) {
    //        state.loadSim  = true;
    //        state.savePath = std::string(saveLoadStr);
    //    } else state.loadSim = false;

    //    ImGui::TreePop();
    //}

    if (ImGui::TreeNode("Frame Stepping")) {
        ImGui::SeparatorText("Frame Stepping");

        static int  framesToStep    = 0;
        static int  pseudoFrames    = 1;
        static bool doFrameStepping = false;

        if (framesToStep > 0 && doFrameStepping) {
            state.runSim = true;
            framesToStep--;
        } else if (doFrameStepping) state.runSim = false;

        ImGui::Text("Frames To Step: %d", pseudoFrames);
        ImGui::SameLine();

        ImGui::PushButtonRepeat(true); // lets you hold down a button to repeat it.
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
            pseudoFrames--;
        }
        ImGui::SameLine();
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
            pseudoFrames++;
        }
        if (ImGui::Button("Step Frames")) {
            framesToStep    = pseudoFrames;
            doFrameStepping = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Disable Frame Stepping")) {
            doFrameStepping = false;
            state.runSim    = true;
        }

        ImGui::PopButtonRepeat(); // Imgui configuration is implemented with a stack? interesting
        ImGui::TreePop();
    }

    ImVec2        windowPos             = ImGui::GetMainViewport()->Pos;
    constexpr int TITLE_BAR_OFFSET_X    = 8;
    constexpr int TITLE_BAR_OFFSET_Y    = 8;
    constexpr int COLOUR_VARIANCE_RANGE = 20;
    state.mouse.x                       = (int)(io.MousePos.x - windowPos.x - TITLE_BAR_OFFSET_X);
    state.mouse.y                       = (int)(io.MousePos.y - windowPos.y - TITLE_BAR_OFFSET_Y);

    if (false) {
        ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode("Debug Values")) {
            ImGui::SeparatorText("Debug Values");
            bool         OutofBounds = false;
            TextureData& texture     = state.textures[loadedTex];
            if (state.mouse.x > texture.width || state.mouse.x < 0 || state.mouse.y > texture.height || state.mouse.y < 0) OutofBounds = true;

            const char* scanMode = Scan::names[state.scanMode].data();

            ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("Application Framecount: %d\n", ImGui::GetFrameCount());
            ImGui::Text("Game Framecount: %d\n", state.frame);
            ImGui::Text("Scale Factor: %d\n", state.scaleFactor);
            ImGui::Text("Textures Reloaded: %d Times\n", state.texReloadCount);
            ImGui::Text("Displayed Texture: %s\n", TexID::names[loadedTex].data());
            ImGui::Text("Texture Width: %d\n", texture.width);
            ImGui::Text("Texture Height: %d\n", texture.height);
            ImGui::Text("Cell Width: %d\n", texture.width / state.scaleFactor);
            ImGui::Text("Cell Height: %d\n", texture.height / state.scaleFactor);
            ImGui::Text("Texture Updates: %d\n", state.textureChanges);
            ImGui::Text("Cell Updates: %d\n", state.cellChanges);
            ImGui::Text("Mouse X: %d\n", state.mouse.x);
            ImGui::Text("Mouse Y: %d\n", state.mouse.y);
            ImGui::Text("Mouse Out of Bounds? %d\n", OutofBounds);

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void Interface::gameWindow(AppState& state) {
    ImGui::Begin("GameWindow");

    loadedTex            = loadedTex % state.textures.size(); // this needs to be here....
    TextureData& texture = state.textures[loadedTex];         // this needs to be here....
    // TODO: Investigage ::GetWindowSize(), get it working for "GameWindow",
    //       Not the SDL2 generated win32 window

    constexpr int xPadding = 10;
    constexpr int yPadding = 20;
    int           windowX  = (int)ImGui::GetWindowSize().x;
    int           windowY  = (int)ImGui::GetWindowSize().y;
    if (windowX % 2 != 0) windowX++;
    if (windowY % 2 != 0) windowY++;

    if (texture.width == 0 || texture.height == 0 || windowX == 0 || windowY == 0) {
        ImGui::SetWindowSize(ImVec2(100, 100));
        texture.width    = 100;
        texture.height   = 100;
        state.reloadGame = true;
    } else if (texture.width + xPadding != windowX || texture.height + yPadding != windowY) {
        if (windowX >= xPadding && windowY >= yPadding) { // prevents unsigned underflow
            texture.width  = windowX - xPadding;
            texture.height = windowY - yPadding;
        }

        if (texture.width % 2 != 0) texture.width++;
        if (texture.height % 2 != 0) texture.height++;

        ImGui::SetWindowSize(ImVec2(texture.width, texture.height));
        state.reloadGame = true;
    } else {
        state.reloadGame = false;
    }

    {
        ImGui::BeginChild("GameRender");
        ImVec2 textureRenderSize = ImVec2(texture.width, texture.height);
        ImGui::Image((ImTextureID)texture.id, textureRenderSize, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}
