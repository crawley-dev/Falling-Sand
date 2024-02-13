#pragma once
#include "pch.h"
#include "state.h"

constexpr u8 CHUNK_SIZE     =  8;
constexpr u8 VARIANT_COUNT  = 20;
constexpr u8 VARIATION      = 12;

struct Material {
    u8 r, g, b, a;
    //u8 dispersion;
    u8 phase;
    u16 density;
    std::vector<std::vector<u8>> variants;

    Material(u8 RED, u8 GREEN, u8 BLUE, u8 ALPHA, /*u8 DISPERSION,*/ u8 PHASE, u16 DENSITY) {
        r = RED;
        g = GREEN;
        b = BLUE;
        a = ALPHA;
        //dispersion	= DISPERSION;
        phase = PHASE;
        density = DENSITY;
        variants = { {RED,GREEN,BLUE,ALPHA} };
    }
    Material() = default;
};

struct Cell {
    u8 matID;
    u8 variant;

    Cell(u8 MATERIAL, u8 COLOUR_VARIANT) {
        matID = MATERIAL;
        variant = COLOUR_VARIANT;
    }
    Cell() = default;
};

struct Chunk {
    //u16 width, height;    // chunk size is fixed at 8x8 << atleast rn
    s32 x, y;               // chunk position in the world
    u64 cellUpdates;        // each cell in a chunk has a bit in this flag.
    bool updated;           // still need a bool for the chunk itself sadge.. （*゜ー゜*）
    std::vector<Cell> cells;

    Chunk(s32 X, s32 Y) {
        x = X;
        y = Y;
        updated = false;
        cellUpdates = 0; // |= 1 << cellidx
        //cells.resize(CHUNK_SIZE * CHUNK_SIZE, Cell(MaterialID::SAND, 0));
    }
    Chunk() = default;
};

// can't really have the concept of an index for a chunk
// as (-x,-y) would wrap to (x,y). && (-x,y) == (x,-y)



class Game {
public:
    void init   (u16 width, u16 height, u8 scale);
    void reload (u16 width, u16 height, u8 scale);
    void update (AppState& state, std::vector<u8>& textureData);
    void reset  ();

    // will draw 1x1 cell for now. to avoid crossing chunk boundaries.
    void mouseDraw(s32 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape);
    void loadImage(std::vector<u8>& textureData, u16 width, u16 height);

private:
    void simulate(AppState& state);

    // 1 loop, change variables based on left or right
    //void l_bottomUp_Update  (std::vector<Cell>& cells); // pass in chunk
    //void r_bottomUp_Update  (std::vector<Cell>& cells); // pass in chunk

    
    //void querySwap          (u16 x1, u16 y1, u16 x2, u16 y2); 
    //void swapCells          (u16 x1, u16 y1, u16 x2, u16 y2);
    //void changeMaterial     (u16 x, u16 y, u8 newMaterial);


    void updateChunk        (Chunk& c);
    void updateCell         (Cell& c); // ref necessary? 
    void updateSand         (Cell& c); // ref necessary? 
    void updateWater        (Cell& c); // ref necessary? 
    void updateNaturalGas   (Cell& c); // ref necessary? 

    void updateEntireTexture(std::vector<u8>& textureData);
    void updateTexture      (std::vector<u8>& textureData);

    //void drawCircle         (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawCircleOutline  (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawLine           (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawSquare         (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawSquareOutline  (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    
    
    // function declarations in header files are implicitly inline :??
    static bool outOfBounds (u8 x, u8 y) { return x >= CHUNK_SIZE || y >= CHUNK_SIZE || x < 0 || y < 0; }
    static u8   cellIdx     (u8 x, u8 y) { return (y * CHUNK_SIZE) + x; }

    Chunk* getChunk(s32 x, s32 y);
    void createChunk(s32 x, s32 y);

    template<typename T> // cheeky template
    T getRand(T min = -1, T max = 1) { return splitMix64_NextRand() % (max - min + 1) + min; }
    u64 splitMix64_NextRand() {
        u64 z = (randSeed += UINT64_C(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }


    /*----------------------------------------------------------------------
    ---- Variables ---------------------------------------------------------
    ----------------------------------------------------------------------*/
    

    bool sizeChanged;
    
    u8 solidDispersion, fluidDispersion, gasDispersion;
    u8 scaleFactor;                             // cell 1x1 --> textur N x N
    s32 cameraX, cameraY;                       // camera position in the world. from top left corner.
    u32 cellWidth, cellHeight;                  // size of render target in cell
    u32 textureWidth, textureHeight;            // size of render target.
    u64 randSeed = 1234567890987654321;

    std::vector<Material> materials;            // material data
    //std::vector<Chunk*> chunkChanges;         // 
    std::vector<Chunk> chunks;                 // for iterating over all chunks
    std::unordered_map<std::pair<s32,s32>, Chunk, boost::hash<std::pair<s32,s32>>> chunkMap;    // for indexing into a chunk

};