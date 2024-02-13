#pragma once
#include "pch.h"
#include "framework.h"

Framework::Framework() {}
Framework::~Framework() {}

bool Framework::init(const char* title, int xpos, int ypos, int width, int height)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return false;
    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) return false; // on success, returns int that the macro expands to, png == 2
    std::cout << Message::names[Message::SDL_INIT] << std::endl;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    std::cout << Message::names[Message::OPENGL_INIT] << std::endl;

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
    if (SDL_GL_SetSwapInterval(-1) != 0) SDL_GL_SetSwapInterval(0);           // Enables Adaptive v-sync if possible, otherwise v-sync
    //SDL_GL_SetSwapInterval(0);                                                  // Disables v-sync
    std::cout << Message::names[Message::WINDOW_INIT] << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();                                                     // Setup Dear ImGui context  
    std::cout << Message::names[Message::IMGUI_CONTEXT_INIT] << std::endl;

    // |= is a bitwise operator: 0101 |= 0110 -> 0111
    ImGuiIO& io = ImGui::GetIO();                                               // Setup ImGui Config
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;                       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;                        // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;                           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;                         // Enable Multi-Viewport / Platform Windows
    io.ConfigDockingWithShift = true;                                           // Enable Docking on Shift
    io.ConfigDockingTransparentPayload = true;                                  // Enable Transparent Window on Docking
    io.Fonts->AddFontFromFileTTF("../Libraries/fonts/Cascadia.ttf", 15);        // Changing Font -> Cascadia Mono (vs editor font) | Relative paths FTW!
    std::cout << Message::names[Message::IMGUI_CONFIG_INIT] << std::endl;

    interface = new Interface();
    if (!interface) return false;
    std::cout << Message::names[Message::INTERFACE_INIT] << std::endl;


    game = new Game();
    if (!game) return false;
    std::cout << Message::names[Message::GAME_INIT] << std::endl;


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


    // Creating Textures.
    state = AppState();
    state.textures.push_back(TextureData(TexID::GAME      , 0, 0, {}));
    state.textures.push_back(TextureData(TexID::BACKGROUND, 0, 0, {}));
    //state.textures.push_back(TextureData(PRESENT_TEXTURE_ID   , 0, 0, {}));

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
    interface->debugMenu(state);

    if (ImGui::GetFrameCount() <= 2) return;

    ImGuiIO& io = ImGui::GetIO();
    TextureData& texture = state.textures[TexIndex::GAME];

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space))) state.runSim = !state.runSim;
    if (io.MouseDown[0]) mouseDraw();
    if (state.resetSim) {
        game->reset();
        state.resetSim = false;
    }

    // shouldn't this be handled inside Game::update()??
    if (state.loadImage) {
        TextureData& img = state.textures[TexIndex::BACKGROUND];
        loadImageRGB(img, state.imagePath);
        game->loadImage(img.data, img.width, img.height);
        state.loadImage = false;
    }
    if (state.reloadGame) {
        reloadTextures();
        game->reload(texture.width, texture.height, state.scaleFactor);
        state.reloadGame = false;
    }

    game->update(state, texture.data);

    for (TextureData& tex : state.textures)
        updateTexture(tex);

    interface->gameWindow(state);

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
        for (TextureData texture : state.textures)
            createTexture(texture);

        TextureData& texture = state.textures[TexIndex::GAME];
        game->init(texture.width, texture.height, state.scaleFactor);
    }

    // Handles Multiple Viewports && Swaps between 2 texture buffers for smoother rendering
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

    for (TextureData& tex : state.textures) 
        glDeleteTextures(1, &tex.id);

    // free heap memory.
    delete game;
    delete interface;

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    std::cout << Message::names[Message::FRAMEWORK_DEAD] << std::endl;
}

//.bmp loading slanted? weird.. 
// TODO: Investigate SDL_ConvertSurfaceFormat
void Framework::loadImageRGB(TextureData& texture, std::string path)
{
    TextureData& gameTexture = state.textures[TexIndex::GAME];

    SDL_Surface* image = IMG_Load(path.c_str());
    if (image == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return;
    }

    constexpr int formatRGB24 = 386930691; // a magic number you might say. constexpr is const but better.
    if (image->format->format != formatRGB24) {
        image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB24, 0);
        if (image->format->format != formatRGB24) { // last chance Surface Convert.
            std::cout << "Unable to load image Pixel Format: " << '\n';
            std::cout << SDL_GetPixelFormatName(image->format->format) << '\n';
            return;
        }
    }

    // if texture is larger than the game area:
    // scale it down, either x2 or --> game area bounds..
    /*if (image->w * image->h * 4 > gameTexture.data.size()) { // doesn't account for long ass images in either w or h..
        SDL_Surface* temp = NULL;
        SDL_Rect destinationRect;
        destinationRect.x = 0;
        destinationRect.y = 0;
        destinationRect.w = gameTexture.width;
        destinationRect.h = gameTexture.height;
        SDL_LowerBlitScaled(image, NULL, temp, &destinationRect);
        *image = *temp; // changes data, not the ptr.
        SDL_FreeSurface(temp);
    }*/

    /* stack overflow to the rescue!! https://stackoverflow.com/questions/40850196/sdl2-resize-a-surface
    if (image->w > GET_GAME_WIDTH || image->h > GET_GAME_HEIGHT) {
         !! big data manipulation wizard to the rescue !!
    }*/


   // have to (un)lock surface to stop SDL2 breaking 
    SDL_LockSurface(image);
    u8* pixel = (u8*)image->pixels;
    SDL_UnlockSurface(image);

    // R | G | B | A 
    // 1 + 1 + 1 + 1 == 4 bytes
    constexpr int bytesPerPixel = 4;
    const int size = image->w * image->h * bytesPerPixel;

    texture.data   = std::vector<u8>(size);
    texture.width  = image->w;
    texture.height = image->h;

    // Mapping RGB --> RGBA
    for (int i = 0; i < size; i += 4) {
        texture.data[i + 0] = *pixel; pixel++;
        texture.data[i + 1] = *pixel; pixel++;
        texture.data[i + 2] = *pixel; pixel++;
        texture.data[i + 3] = 255;
    }

    SDL_FreeSurface(image);
    updateTexture(texture);
    
    // downscaling texture if necessary
    //TextureData& gameTexture = data.textures[GAME_TEXTURE_ID];
    //while (texture.data.size() > gameTexture.data.size()) {
    //    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    //    glBindTexture(GL_TEXTURE_2D, texture.id);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 2); // downscale x2
    //    glGenerateMipMap()
    //}
}

void Framework::loadImageRGBA(TextureData& texture, std::string path)
{
    //TextureData& texture = data.textures[TexIndex::BACKGROUND];
    SDL_Surface* image = IMG_Load(path.c_str());
    if (image == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return;
    }
    image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA8888, 0); // 24 bit --> 32 bit
    SDL_PixelFormat* fmt = image->format;
    std::cout << SDL_GetPixelFormatName(fmt->format) << std::endl;
    printf("Bytes Per Pixel: %d \n", fmt->BytesPerPixel);

    SDL_LockSurface(image);
    Uint32* pixels = (Uint32*)image->pixels;
    SDL_UnlockSurface(image);

    texture.width = image->w;
    texture.height = image->h;
    texture.data = std::vector<GLubyte>(texture.width * texture.height * fmt->BytesPerPixel);
    int temp = 0;
    
    for (int y = 0; y < texture.height; y++) 
        for (int x = 0; x < texture.width; x++) { 
            // A Uint32 is 4 bytes, so each time you step your byte offset forward by 1 pixel by incrementing x by 1, 
            // your index into the pixel data leaps forward 4 * 4 = 16 bytes:
            u32 i = (y * texture.width) + x;
            u32 pixel = pixels[i];
    
            /* Get Red component */
            temp = pixel & fmt->Rmask;  /* Isolate red component */
            temp = temp >> fmt->Rshift; /* Shift it down to 8-bit */
            temp = temp << fmt->Rloss;  /* Expand to a full 8-bit number */
            texture.data[i + 0] = (Uint8)temp;
    
            /* Get Green component */
            temp = pixel & fmt->Gmask;  /* Isolate green component */
            temp = temp >> fmt->Gshift; /* Shift it down to 8-bit */
            temp = temp << fmt->Gloss;  /* Expand to a full 8-bit number */
            texture.data[i + 1] = (Uint8)temp;
    
            /* Get Blue component */
            temp = pixel & fmt->Bmask;  /* Isolate blue component */
            temp = temp >> fmt->Bshift; /* Shift it down to 8-bit */
            temp = temp << fmt->Bloss;  /* Expand to a full 8-bit number */
            texture.data[i + 2] = (Uint8)temp;
    
            /* Get Alpha component */
            temp = pixel & fmt->Amask;  /* Isolate alpha component */
            temp = temp >> fmt->Ashift; /* Shift it down to 8-bit */
            temp = temp << fmt->Aloss;  /* Expand to a full 8-bit number */
            texture.data[i + 3] = temp;
        }

    SDL_FreeSurface(image);
    updateTexture(texture);
}


// C:/Users/Tom/source/repos/TheCookiess/PixelPhysV2/Resources/Saves
void Framework::saveToFile(TextureData& texture)
{
    
}

void Framework::loadFromFile(TextureData& texture, std::string path)
{

}

void Framework::createTexture(TextureData& texture)
{
    texture.data = std::vector<GLubyte>(texture.width * texture.height * 4, 255);

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // GL_LINEAR --> GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // FASTER but produces blocky, pixelated texture (not noticeable-ish)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// GL_CLAMP_TO_EDGE == the default behaviour of texture wrapping. (don't need it)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// GL_TEXTURE_WARP_S or T == sets wrapping behaviour of texture beyond regular size
}

void Framework::reloadTextures()
{
    for (TextureData& texture : state.textures) {
        glDeleteTextures(1, &texture.id);
        createTexture(texture);
    }
    state.texReloadCount++;
}

void Framework::updateTexture(TextureData& texture)
{ 
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, texture.data.data()); // data.data, weird..
}

void Framework::mouseDraw()
{
    ImGuiIO& io = ImGui::GetIO();

    static int lastFrameCall   = 0;
    constexpr int minFramesTilDraw = 5;
    if (ImGui::GetFrameCount() - lastFrameCall > minFramesTilDraw) {
        lastFrameCall = ImGui::GetFrameCount();
        return;
    } 
    
    // Mouse pos updated in interface->debugMenu() each frame. called before mouseDraw event so correct.
    game->mouseDraw(state.mouseX, state.mouseY, state.drawSize, state.drawChance, state.drawMaterial, state.drawShape);
}