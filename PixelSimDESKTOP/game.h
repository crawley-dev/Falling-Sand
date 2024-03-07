#pragma once
#include "pch.h"
#include "state.h"

constexpr u8 CHUNK_SIZE    = 16;
constexpr u8 VARIANT_COUNT = 20;
constexpr u8 VARIATION     = 12;

// type aliasing to better define coordinate space
using cellSpace     = u8;
using viewportSpace = u16;
using chunkSpace    = s32;

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

    Chunk(s32 X, s32 Y, u8 material) {
        x           = X;
        y           = Y;
        cellUpdates = 0; // |= 1 << cellidx
        cells       = std::vector<Cell>();
    }
    Chunk() = default;
};

struct Material {
    u8                           r, g, b, a;
    u16                          density;
    std::vector<std::vector<u8>> variants;

    Material(u8 RED, u8 GREEN, u8 BLUE, u8 ALPHA, u16 DENSITY) {
        r        = RED;
        g        = GREEN;
        b        = BLUE;
        a        = ALPHA;
        density  = DENSITY;
        variants = {{RED, GREEN, BLUE, ALPHA}};
    }
    Material() = default;
};

class Game {
public:
    Game();
    ~Game();

    void init(u16 width, u16 height, u8 scale);
    void reload(u16 width, u16 height, u8 scale);
    void update(AppState& state, std::vector<u8>& textureData);
    void reset();

    // will draw 1x1 cell for now. to avoid crossing chunk boundaries.
    void mouseDraw(AppState& state, s32 x, s32 y, u16 size, u8 drawChance, u8 material, u8 shape);
    void loadImage(std::vector<u8>& textureData, u16 width, u16 height);

private:
    void simulate(AppState& state);

    // 1 loop, change variables based on left or right
    //void l_bottomUpUpdate(Chunk& chunk);
    //void r_bottomUpUpdate(Chunk& chunk);

    // void querySwap          (u16 x1, u16 y1, u16 x2, u16 y2);
    // void swapCells          (u16 x1, u16 y1, u16 x2, u16 y2);
    //void changeMaterial(u16 x, u16 y, u8 newMaterial);
    void changeMaterial(s32 x, s32 y, u8 newMaterial);

    Chunk* getChunk(s16 x, s16 y);
    Chunk* createChunk(s16 x, s16 y, u8 material = 0);

    // void updateChunk(Chunk &c);
    // bool updateCell(Cell& c, u16 x, u16 y); // ref necessary?
    // void updateSand(Cell &c);       // ref necessary?
    // void updateWater(Cell &c);      // ref necessary?
    // void updateNaturalGas(Cell &c); // ref necessary?

    void updateEntireTexture(std::vector<u8>& textureData);
    void updateTexture(std::vector<u8>& textureData);

    void drawCircle(s32 x, s32 y, u16 size, u8 material, u8 drawChance, std::function<void(s32, s32)> foo);
    // void drawCircleOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    // void drawLine(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    // void drawSquare(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    // void drawSquareOutline(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawLine(s32 x1, s32 y1, s32 x2, s32 y2, std::function<void(s32, s32)>& foo);


    bool outOfChunkBounds(u8 x, u8 y) const;      // relative to CHUNK_SIZE constexpr value
    bool outOfTextureBounds(u32 x, u32 y) const;  // relative to textureWidth
    bool outOfViewportBounds(s32 x, s32 y) const; // relative to viewport position (camera)
    bool outOfCellBounds(s32 x, s32 y) const;     // relative to ??


    // generic-alise this? // std::pair<T,T> replaced
    Coord<s16> worldToChunk(s32 x, s32 y) const;                 // world -> chunk
    Coord<s32> viewportToWorld(s32 x, s32 y) const;              // viewport x,y -> world space.. (accounts for camera)
    Coord<s32> mouseToWorld(s32 x, s32 y) const;                 // mouse x,y -> world space.. (reverse accounts for camera)
    Coord<s32> chunkToWorld(s16 cX, s16 cY, u8 lX, u8 lY) const; // chunk space -> world space
    Coord<u8>  worldToCell(s32 x, s32 y) const;
    Coord<u8>  chunkToCell(s32 x, s32 y, s16 cX, s16 cY) const;

    u8  cellIdx(u8 x, u8 y) const;      // gets index into chunk->cells
    u32 textureIdx(u16 x, u16 y) const; // gets index into textureData << accepts viewport coords.

    template <typename T>
    T   getRand(T min, T max);
    u64 splitMix64_NextRand();

    /*----------------------------------------------------------------------
    ---- Variables ---------------------------------------------------------
    ----------------------------------------------------------------------*/

    bool sizeChanged;

    u8 solidDispersion, fluidDispersion, gasDispersion;
    u8 scaleFactor; // cell 1x1 --> texture N x N
    //s32 cameraX, cameraY;               // camera posistion in the world. from top left corner.
    //s32 cellWidth, cellHeight;          // size of render target in cell
    //s32 textureWidth, textureHeight;    // size of render target.
    Coord<s32> camera;
    Coord<s32> cellSize;
    Coord<s32> textureSize;
    u64        randSeed = 1234567890987654321; // set random seed

    std::vector<std::pair<std::vector<u8>, std::pair<s32, s32>>>                      textureChanges; // render buffer changes.
    std::vector<Chunk*>                                                               updatedChunks;  // to update
    std::vector<Material>                                                             materials;      // material data
    std::vector<Chunk*>                                                               chunks;         // holds all chunks, for deleting
    std::unordered_map<std::pair<s16, s16>, Chunk*, boost::hash<std::pair<s16, s16>>> chunkMap;       // for indexing into a chunk
};
