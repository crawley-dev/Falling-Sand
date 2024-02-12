#pragma once
#include "pch.h"
#include "state.h"

constexpr u8 CHUNK_SIZE = 8;

struct Cell {
    u8 matID;
    u8 variant;

    Cell(u8 MATERIAL, u8 COLOUR_VARIANT) {
        matID = MATERIAL;
        variant = COLOUR_VARIANT;
    }
};

struct Chunk {
    //u16 width, height;    // chunk size is fixed at 8x8 << atleast rn
    s32 x, y;               // chunk position in the world
    u64 cellUpdates = 0;    // each cell in a chunk has a bit in this flag.
    bool updated = false;   // still need a bool for the chunk itself sadge.. （*゜ー゜*）
    std::vector<Cell> cells;

    Chunk(s32 X, s32 Y) {
        x = X;
        y = Y;
        cells.resize(CHUNK_SIZE * CHUNK_SIZE, Cell(MaterialID::EMPTY, 0));
    }
};

// can't really have the concept of an index for a chunk
// as (-x,-y) would wrap to (x,y). && (-x,y) == (x,-y)

/* 
First Implementation :
    - get the chunk at the camera position
    - calculate how many chunks are visible in x & y ((textureWidth / scaleFactor) / CHUNK_SIZE)
    - loop through from camera Chunk to Final Chunk
        - get chunk from map
        - update chunk's cells bot -> top
*/

class Simulation {
public:
    void init();
    void reload();
    void update();
    void reset();

    // will draw 1x1 cell for now. to avoid crossing chunk boundaries.
    void mouseDraw(s32 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape);

private:
    void simulate();

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


    //void drawCircle         (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawCircleOutline  (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawLine           (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawSquare         (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    //void drawSquareOutline  (u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);

    inline bool outOfBounds(u8 x, u8 y) const { return x >= CHUNK_SIZE || y >= CHUNK_SIZE || x < 0 || y < 0; }
    inline u8 cellIdx(u8 x, u8 y) const { return (y * CHUNK_SIZE) + x; }

    
    /*----------------------------------------------------------------------
    ---- Variables ---------------------------------------------------------
    ----------------------------------------------------------------------*/
    
    
    u8 scaleFactor;                             // cell 1x1 --> textur N x N
    s32 cameraX, cameraY;                       // camera position in the world. from top left corner.
    u32 textureWidth, textureHeight;            // size of render target.

    //std::vector<Chunk*> chunkChanges;           // 
    std::vector<Chunk*> chunks;                 // for iterating over all chunks
    std::unordered_map<std::pair<s32,s32>, Chunk*> chunkMap;    // for indexing into a chunk

};