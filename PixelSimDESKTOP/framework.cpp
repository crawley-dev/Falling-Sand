#pragma once

#include "game.h"
#include "framework.h"
#include "interface.h"
    
//#include "interfaceData.h"

Framework::Framework() {}
Framework::~Framework() {}

bool Framework::init(const char* title, int xpos, int ypos, int width, int height)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return false;
    //if (IMG_Init(IMG_INIT_JPG) != 1) return false;
    std::cout << "SDL Initialised! .. " << std::endl;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    std::cout << "OpenGL Initialised! .." << std::endl;

    // Enable Native Support for Non QWERTY Input (e.g Japanese Kanji)
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); 
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create SDL Window, integrate with openGL
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window  = SDL_CreateWindow(title, xpos, ypos, width, height, window_flags); // Create SDL Window
    gl_context  = SDL_GL_CreateContext(window);                                 // Create openGL Context
    SDL_GL_MakeCurrent(window, gl_context);                                     // Set SDL_Window Context
    if (SDL_GL_SetSwapInterval(-1) != 0) SDL_GL_SetSwapInterval(0);             // Enables Adaptive v-sync if possible, otherwise v-sync
    std::cout << "Window Initialised! .." << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();                                                     // Setup Dear ImGui context  
    std::cout << "ImGui Context Initialised! .." << std::endl;

    // |= is a bitwise operator: 0101 |= 0110 -> 0111
    ImGuiIO& io = ImGui::GetIO();                                               // Setup ImGui Config
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;                       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;                        // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;                           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;                         // Enable Multi-Viewport / Platform Windows
    io.ConfigDockingWithShift = true;                                           // Enable Docking on Shift
    io.ConfigDockingTransparentPayload = true;                                  // Enable Transparent Window on Docking
    io.Fonts->AddFontFromFileTTF("../Libraries/fonts/Cascadia.ttf", 15);        // Changing Font -> Cascadia Mono (vs editor font) | Relative paths FTW!
    std::cout << "ImGui Config Initialised! .." << std::endl;
    
    interface = new Interface();
    if (!interface) return false;
    std::cout << "Interface Initialised! .." << std::endl;

    game = new Game();
    if (!game) return false;
    std::cout << "Game initialised! .." << std::endl;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    std::cout << "Setup Complete! .." << std::endl;
    std::cout << std::endl;

    data = interfaceData();
    data.textures.push_back(TextureData(GAME_TEXTURE_IDX, 0, 0, {}));
    data.textures.push_back(TextureData(BACKGROUND_TEXTURE_IDX, 0, 0, {}));
    
    applicationRunning = true;
    return true;
}

void Framework::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            applicationRunning = false;
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            applicationRunning = true;
    }
}

void Framework::update()
{
    interface->main();
    interface->debugMenu(data);

    if (ImGui::GetFrameCount() < 2) return;
    ImGuiIO& io = ImGui::GetIO();
    TextureData& texture = data.textures[GAME_TEXTURE_IDX];

    data.drawSize           += (int)io.MouseWheel;
    data.drawSize           = std::clamp(data.drawSize          , 1,1000);
    data.drawChance         = std::clamp(data.drawChance        , 1, 100);
    data.drawColourVariance = std::clamp(data.drawColourVariance, 1, 255);
    data.scaleFactor        = std::clamp(data.scaleFactor       , 1,  10);

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space))) data.runSim = !data.runSim;
    if (io.MouseDown[0]) mouseDraw();
    if (data.resetSim) game->reset(data.resetSim);
    if (data.reloadGame) {
        reloadTextures();
        game->reload(texture.data, texture.width, texture.height, data.scaleFactor);
        data.reloadGame = false;
    }
    game->update(data);

    texture.data = game->getTextureData();
    //for (int i = 0; i < data.textures.size() - 1; i++)
       // updateTexture(i);
    updateTexture(GAME_TEXTURE_IDX);

    interface->gameWindow(data);
}

void Framework::render()
{
    ImGuiIO& io = ImGui::GetIO();

    // Main rendering
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Placed After ImGui::Render() to prevent ImGui HUD overwriting my textures.
    if (ImGui::GetFrameCount() == 2) {
        createTexture(GAME_TEXTURE_IDX);
        createTexture(BACKGROUND_TEXTURE_IDX);
        TextureData texture = data.textures[GAME_TEXTURE_IDX];
        game->init(texture.data, texture.width, texture.height, data.scaleFactor);
    }

    // Handling Multiple Viewports && Swaps between 2 texture buffers for smoother rendering
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
    
    SDL_GL_SwapWindow(window);  
}

void Framework::clean()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    //glDeleteTextures(1, &data.textures.id);

    // delete all existing textures.
    for (TextureData& tex : data.textures)
        glDeleteTextures(1, &tex.id);

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    std::cout << "Game Cleaned! .." << std::endl;
}

void Framework::createTexture(int id)
{
    // basically just reloading textures..
    TextureData& texture = data.textures[id];

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // GL_LINEAR --> GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // FASTER but produces blocky, pixelated texture (not noticeable-ish)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// GL_CLAMP_TO_EDGE == the default behaviour of texture wrapping. (don't need it)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// GL_TEXTURE_WARP_S or T == sets wrapping behaviour of texture beyond regular size

    texture.data = std::vector<GLubyte>(texture.width * texture.height * 4, 255);

    updateTexture(id);
}

void Framework::reloadTextures()
{
    // texture id 0 & 1 are taken. My textures start indexing at 2
    for (int i = 0; i < data.textures.size() - 1; i++) {
        glDeleteTextures(1, &data.textures[i].id); // delete old textures
        createTexture(i); // create new texture.
        updateTexture(i); // push to renderer
    }
    data.texReloadCount++;
}

void Framework::updateTexture(int id)
{ 
    TextureData texture = data.textures[id];
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, texture.data.data()); // data.data, weird..
}

void Framework::mouseDraw()
{
    ImGuiIO& io = ImGui::GetIO();

    static int lastFrameCall   = 0;
    const int minFramesTilDraw = 5;
    if (ImGui::GetFrameCount() - lastFrameCall > minFramesTilDraw) {
        lastFrameCall = ImGui::GetFrameCount();
        return;
    } 
    
    // Mouse pos updated in interface->debugMenu() each frame. called before mouseDraw event so correct.
    game->mouseDraw(data.mouseX, data.mouseY, data.drawSize, data.drawChance, data.drawType, data.drawShape, data.drawColourVariance);
}