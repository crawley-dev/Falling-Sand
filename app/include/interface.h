#pragma once
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "state.h"

class Interface {
public:
    Interface();
    ~Interface();

    void        main();
    void        boilerPlate();
    void        debugMenu(AppState& state);
    void        gameWindow(AppState& state);
    inline void demoWindow() {
        if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
    }

private:
    bool     showDemoWindow = true;
    int      loadedTex      = 0;
    ImGuiIO& io             = ImGui::GetIO();
};
