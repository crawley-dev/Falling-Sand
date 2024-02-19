#pragma once
#include "pch.h"
#include "state.h"

constexpr u8 CHUNK_SIZE    = 16;
constexpr u8 VARIANT_COUNT = 20;
constexpr u8 VARIATION     = 12;

struct Material {
    u8                           r, g, b, a;
    u16                          density;
    std::vector<std::vector<u8>> variants;

    Material(u8 RED, u8 GREEN, u8 BLUE, u8 ALPHA, u16 DENSITY) {
        r = RED;
        g = GREEN;
        b = BLUE;
        a = ALPHA;
        // dispersion	= DISPERSION;
        density  = DENSITY;
        variants = {{RED, GREEN, BLUE, ALPHA}};
    }
    Material() = default;
};

struct Cell {
    u8 matID;
    u8 variant;

    Cell(u8 MATERIAL, u8 COLOUR_VARIANT) {
        matID   = MATERIAL;
        variant = COLOUR_VARIANT;
    }
    Cell() = default;
};

struct Chunk {
    s32 x, y;        // chunk position in the world
    u64 cellUpdates; // each cell in a chunk has a bit in this flag.
    //bool updated;    // still need a bool for the chunk itself sadge.. （*゜ー゜*）
    // maybe i don't need it! if cellUpdates == 0, no cells need updating therefore skip!
    std::vector<Cell> cells;

    Chunk(s32 X, s32 Y) {
        x           = X;
        y           = Y;
        cellUpdates = 0; // |= 1 << cellidx
        cells.resize(CHUNK_SIZE * CHUNK_SIZE, Cell(MaterialID::SAND, 0));
    }
    Chunk() = default;
};

class Game {
public:
    void init(u16 width, u16 height, u8 scale);
    void reload(u16 width, u16 height, u8 scale);
    void update(AppState& state, std::vector<u8>& textureData);
    void reset();

    // will draw 1x1 cell for now. to avoid crossing chunk boundaries.
    void mouseDraw(s32 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape);
    void loadImage(std::vector<u8>& textureData, u16 width, u16 height);

private:
    void simulate(AppState& state);

    // 1 loop, change variables based on left or right
    //void l_bottomUpUpdate(Chunk& chunk);
    //void r_bottomUpUpdate(Chunk& chunk);

    // void querySwap          (u16 x1, u16 y1, u16 x2, u16 y2);
    // void swapCells          (u16 x1, u16 y1, u16 x2, u16 y2);
    // void changeMaterial     (u16 x, u16 y, u8 newMaterial);

    inline Chunk* getChunk(s16 x, s16 y);
    inline Chunk* createChunk(s16 x, s16 y);

    // void updateChunk(Chunk &c);
    //bool updateCell(Cell& c, u16 x, u16 y); // ref necessary?
    // void updateSand(Cell &c);       // ref necessary?
    // void updateWater(Cell &c);      // ref necessary?
    // void updateNaturalGas(Cell &c); // ref necessary?

    void updateEntireTexture(std::vector<u8>& textureData);
    void updateTexture(std::vector<u8>& textureData);

    // void drawCircle(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    // void drawCircleOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    // void drawLine(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    // void drawSquare(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    // void drawSquareOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);

    //void drawLine(Chunk* src, u8 srcX, u8 srcY, Chunk* dest, u8 destX, u8 destY);
    void drawLine(s32 x1, s32 y1, s32 x2, s32 y2);

    bool outofCameraBounds(u8 x, u8 y) const {
        return x >= (cellWidth + cameraX) || y >= (cellHeight + cameraY) || x < cameraX || y < cameraX;
    }
    bool                       outOfBounds(s32 x, s32 y) const { return x >= cellWidth || y >= cellHeight || x < 0 || y < 0; }
    static bool                outOfChunkBounds(u8 x, u8 y) { return x >= CHUNK_SIZE || y >= CHUNK_SIZE || x < 0 || y < 0; }
    static std::pair<s16, s16> toChunkCoords(s32 x, s32 y) { return {floor(float(x) / CHUNK_SIZE), floor(float(y) / CHUNK_SIZE)}; }
    static u8                  cellIdx(u8 x, u8 y) { return (y * CHUNK_SIZE) + x; }
    u32                        textureIdx(u16 x, u16 y) const { return 4 * ((y * textureWidth) + x); }


    template <typename T> // cheeky template
    inline T getRand(T min = -1, T max = 1) {
        return splitMix64_NextRand() % (max - min + 1) + min;
    }
    inline u64 splitMix64_NextRand() {
        u64 z = (randSeed += UINT64_C(0x9E3779B97F4A7C15));
        z     = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z     = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }

    /*----------------------------------------------------------------------
    ---- Variables ---------------------------------------------------------
    ----------------------------------------------------------------------*/

    bool sizeChanged;

    u8  solidDispersion, fluidDispersion, gasDispersion;
    u8  scaleFactor;                 // cell 1x1 --> texture N x N
    s32 cameraX, cameraY;            // camera posistion in the world. from top left corner.
    s32 cellWidth, cellHeight;       // size of render target in cell
    s32 textureWidth, textureHeight; // size of render target.
    u64 randSeed = 1234567890987654321;

    //std::vector<Chunk*>                                                               updatedChunks; // to update
    std::vector<std::pair<std::vector<u8>, std::pair<s32, s32>>>                      textureChanges;
    std::vector<Material>                                                             materials; // material data
    std::vector<Chunk*>                                                               chunks;    // holds all chunks, for deleting
    std::unordered_map<std::pair<s16, s16>, Chunk*, boost::hash<std::pair<s16, s16>>> chunkMap;  // for indexing into a chunk
};
