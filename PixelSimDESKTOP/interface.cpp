#pragma once

#include "interface.h"

Interface::Interface() {}
Interface::~Interface() {}

void Interface::main(GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount)
{
    io = ImGui::GetIO(); // keep IO upto date

    boilerPlate();
    demoWindow();
    debugMenu(textureID, textureWidth, textureHeight, texReloadedCount);
}

void Interface::boilerPlate()
{
    ImGui_ImplOpenGL3_NewFrame();   // init openGl frame  
    ImGui_ImplSDL2_NewFrame();      // init SDL2   frame
    ImGui::NewFrame();              // init imgui  frame

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()); // add background dockspace by default
}

void Interface::debugMenu(GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount)
{   // A Begin/End Pair (the brackets) to contain a named window's creation and destruction. required!!
    ImGui::Begin("Debug Menu");                                     // Create a window called "Hello, world!" and append into it  
    ImGui::Checkbox("Toggle Demo Window", &showDemoWindow);         // Edit bool toggling on/off the demo window

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
    ImGui::Text("textureID:%d\n", textureID);
    ImGui::Text("Reloaded Texture: %d Times\n", texReloadedCount);
    ImGui::Text("textureWidth: %d\n", textureWidth);
    ImGui::Text("textureHeight: %d\n", textureHeight);

    ImGui::End();
}

void Interface::gameWindow(GLuint textureID, int& textureWidth, int& textureHeight, bool& hasSizeChanged)
{
    ImGui::Begin("GameWindow");
    frameRate = io.Framerate;

    if (textureWidth != (int)ImGui::GetWindowSize().x || textureHeight != (int)ImGui::GetWindowSize().y) {
        hasSizeChanged = true;
        textureWidth = ImGui::GetWindowSize().x;
        textureHeight = ImGui::GetWindowSize().y;
    }
    else hasSizeChanged = false;

    {
        ImGui::BeginChild("GameRender");
        ImGui::Image((ImTextureID)textureID, ImVec2(512, 512), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
        ImGui::EndChild();
    }
    ImGui::End();
}