#pragma once
#include "state.h"
#include <functional>

struct Cell {     // 32 bits of data, for more cache hits === speed.
    bool updated; // uses 1 byte??? should just be a bit
    u8   matID;
    u8   variant; // index to array of randomly generated RGBA values from material's RGBA.
    u8   data;    // extra data if needed, e.g fire temp

    Cell(bool UPDATED, u8 MATERIAL, u8 COLOUR_VARIANT, u8 EXTRA_DATA) {
        updated = UPDATED;
        matID   = MATERIAL;
        variant = COLOUR_VARIANT;
        data    = EXTRA_DATA;
    }
    Cell() = default;
};

struct Material {
    bool                         movable;
    u8                           r, g, b, a;
    u8                           dispersion;
    u16                          density;
    std::vector<std::vector<u8>> variants;

    Material(u8 RED, u8 GREEN, u8 BLUE, u8 ALPHA, u8 DISPERSION, u16 DENSITY, bool MOVABLE) {
        r          = RED;
        g          = GREEN;
        b          = BLUE;
        a          = ALPHA;
        dispersion = DISPERSION;
        density    = DENSITY;
        variants   = {{RED, GREEN, BLUE, ALPHA}};
        movable    = MOVABLE;
    }
    Material() = default;
};

class Game {
public:
    Game();
    ~Game();

    void init(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor);
    void reload(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor);
    void update(AppState& state, std::vector<u8>& textureData);
    void reset();

    void loadImage(std::vector<u8>& textureData, std::vector<u8>& imageTextureData, u16 imageWidth, u16 imageHeight);

    void mouseDraw(u16 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape);

private:
    void simulate(AppState& state);

    void l_bottomUpUpdate();
    void r_bottomUpUpdate();
    void snakeUpdate();
    void golUpdate();

    void changeMaterial(u16 x, u16 y, u8 newMaterial);
    void swapCells(u16 x1, u16 y1, u16 x2, u16 y2);
    bool querySwap(u16 x1, u16 y1, u16 x2, u16 y2);
    bool querySwapAbove(u16 x1, u16 y1, u16 x2, u16 y2);

    bool updateCell(u16 x, u16 y);
    bool updateSand(u16 x, u16 y);
    bool updateWater(u16 x, u16 y);
    bool updateNaturalGas(u16 x, u16 y);

    void updateTextureData(std::vector<u8>& textureData);
    void updateEntireTextureData(std::vector<u8>& textureData);

    void createDrawIndicators(u16 x, u16 y, u16 size, u8 shape);

    void drawCircle(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawCircleOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawLine(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawSquare(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawSquareOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);

    bool outOfBounds(u16 x, u16 y) const { return x >= cellWidth || y >= cellHeight || x < 0 || y < 0; }
    u32  cellIdx(u16 x, u16 y) const { return (y * cellWidth) + x; }
    u32  textureIdx(u16 x, u16 y) const { return 4 * ((y * textureWidth) + x); }

    template <typename T> // cheeky template
    inline T getRand(T min = -1, T max = 1) {
        return splitMix64_NextRand() % (max - min + 1) + min;
    }
    u64 splitMix64_NextRand() {
        u64 z = (seed += UINT64_C(0x9E3779B97F4A7C15));
        z     = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z     = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }


    /*----------------------------------------------------------------
    ---- Variables ---------------------------------------------------
    ----------------------------------------------------------------*/


    bool sizeChanged = false;

    u8 gasDispersionFactor;
    u8 fluidDispersionFactor;
    u8 solidDispersionFactor;

    u8  nVariants;
    u8  scaleFactor;
    u16 textureWidth, textureHeight;
    u16 cellWidth, cellHeight;
    u64 seed = 1234567890987654321;

    std::vector<Cell>                cells;
    std::vector<Material>            materials;
    std::vector<std::pair<u16, u16>> textureChanges;
    std::vector<std::pair<u16, u16>> drawIndicators;
};
