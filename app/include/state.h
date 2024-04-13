#pragma once
#define SDL_MAIN_HANDLED
#define DIST_MODE 1
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

// Type Definitions
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned long int;
using u64 = unsigned long long;

using s8  = char;
using s16 = short;
using s32 = long int;
using s64 = long long;

using f32 = float;
using f64 = double;

using GLubyte = unsigned char;
using GLuint  = unsigned int;

struct Message {
    enum : u8 {
        SDL_INIT,
        OPENGL_INIT,
        WINDOW_INIT,
        IMGUI_CONTEXT_INIT,
        IMGUI_CONFIG_INIT,
        INTERFACE_INIT,
        GAME_INIT,
        FRAMEWORK_DEAD,
        COUNT,
    };

    static constexpr std::array<std::string_view, Message::COUNT> names{
        "[Pixel Sim]           SDL .. [INIT]",
        "[Pixel Sim]        OpenGL .. [INIT]",
        "[Pixel Sim]        Window .. [INIT]",
        "[Pixel Sim] ImGui Context .. [INIT]",
        "[Pixel Sim]  ImGui Config .. [INIT]",
        "[Pixel Sim]     Interface .. [INIT]",
        "[Pixel Sim]          Game .. [INIT]",
        "[Pixel Sim]     Framework .. [DEAD]",
    };
};


struct MaterialID {
    enum : u8 {
        EMPTY,
        CONCRETE,
        SAND,
        WATER,
        NATURAL_GAS,
        FIRE,
        GOL_ALIVE,
        COUNT,
    };

    static constexpr std::array<std::string_view, MaterialID::COUNT> names{
        "EMPTY",
        "CONCRETE",
        "SAND",
        "WATER",
        "Natural Gas",
        "FIRE",
        "Game of Life: Alive",
    };
};

struct Flag {
    enum : u64 {
        RUN_SIM     = 1 << 0,
        RESET_SIM   = 1 << 1,
        RELOAD_GAME = 1 << 2,
        LOAD_IMAGE  = 1 << 3,
    };
};

struct TexIndex {
    enum : u8 {
        GAME,
        BACKGROUND,
        PRESENTED,
        COUNT,
    };
};

struct TexID {
    enum : u8 {
        GAME = 2,
        BACKGROUND,
        PRESENTED,
        COUNT,
    };

    static constexpr std::array<std::string_view, TexID::COUNT - 2> names{
        "Game",
        "Background",
        "Presented",
    };
};

struct Update {
    enum : u8 {
        STATIC,
        CYCLE,
        COUNT,
    };

    static constexpr std::array<std::string_view, Update::COUNT> names{
        "Static",
        "CYCLE",
    };
};

struct Scan {
    enum : u8 {
        BOTTOM_UP_L,
        BOTTOM_UP_R,
        TOP_DOWN_L,
        TOP_DOWN_R,
        SNAKE,
        GAME_OF_LIFE,
        COUNT,
    };

    static constexpr std::array<std::string_view, Scan::COUNT> names{
        "Bottom Up L->R",
        "Bottom Up R->L",
        "Top Down L->R",
        "Top Down R->L",
        "Snake",
        "Game of Life",
    };
};


struct Shape {
    enum : u8 {
        CIRCLE,
        CIRCLE_OUTLINE,
        LINE,
        SQUARE,
        SQUARE_OUTLINE,
        COUNT,
    };

    static constexpr std::array<std::string_view, Shape::COUNT> names{
        "Circle",
        "Circle Outline",
        "Line",
        "Square",
        "Square Outline",
    };
};

struct TextureData {
    GLuint          id     = 0; // can't be u8 because ptrs.
    u16             width  = 0;
    u16             height = 0;
    std::vector<u8> data;

    TextureData(u32 ID, u16 WIDTH, u16 HEIGHT, std::vector<u8> DATA) {
        id     = ID;
        width  = WIDTH;
        height = HEIGHT;
        data   = DATA;
    }
    TextureData() = default;
};

//struct HoverData {};

template <typename T>
struct Vec2 {
    T x, y;

    Vec2(T _x, T _y) {
        x = _x;
        y = _y;
    }
    Vec2(T _n) {
        x = _n;
        y = _n;
    }
    Vec2() = default;

    Vec2<T> operator+(const Vec2& c) {
        return Vec2<T>(x + c.x, y + c.y);
    }

    Vec2<T> operator-(const Vec2& c) {
        return Vec2<T>(x - c.x, y - c.y);
    }

    Vec2<T> operator*(const Vec2& c) {
        return Vec2<T>(x * c.x, y * c.y);
    }

    Vec2<T> operator/(const Vec2& c) {
        return Vec2<T>(x / c.x, y / c.y);
    }

    Vec2<T>& operator+=(const Vec2<T>& c) {
        x += c.x;
        y += c.y;
        return *this;
    }

    Vec2<T>& operator-=(const Vec2<T>& c) {
        x -= c.x;
        y -= c.y;
        return *this;
    }

    Vec2<T>& operator*=(const T& scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2<T>& operator/=(const T& scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
};

struct AppState {
    std::vector<TextureData> textures;
    std::string              imagePath;
    std::string              savePath;


    bool saveSim    = false;
    bool loadSim    = false;
    bool runSim     = false;
    bool resetSim   = false;
    bool reloadGame = false;
    bool loadImage  = false;

    u8 scanMode              = Scan::BOTTOM_UP_L;
    u8 updateMode            = Update::CYCLE;
    u8 drawShape             = Shape::SQUARE;
    u8 drawMaterial          = MaterialID::WATER;
    u8 drawChance            = 50;
    u8 scaleFactor           = 5;
    u8 solidDispersionFactor = 2;
    u8 fluidDispersionFactor = 6;
    u8 gasDispersionFactor   = 8;

    Vec2<u16> mouse    = Vec2<u16>(0, 0);
    u16       drawSize = 10;

    u32 frame          = 0;
    u32 texReloadCount = 0;
    u32 textureChanges = 0;
    u32 cellChanges    = 0;

    Vec2<s32> camera = Vec2<s32>(0, 0);

    Vec2<s32> print_mouse    = Vec2<s32>(0, 0);
    Vec2<s32> print_world    = Vec2<s32>(0, 0);
    Vec2<s32> print_viewport = Vec2<s32>(0, 0);
    Vec2<s32> print_chunk    = Vec2<s32>(0, 0);
    Vec2<s32> print_cell     = Vec2<s32>(0, 0);
};
