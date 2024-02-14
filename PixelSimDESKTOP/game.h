#pragma once
#include "pch.h"
#include "cell.h"
#include "state.h"

class Game {
public:
    Game();
    ~Game();

    void init(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor);
    void reload(u16 newTextureWidth, u16 newTextureHeight, u8 newScaleFactor);
    void update(AppState& state, std::vector<u8>& textureData);
    void reset();

    void loadImage(std::vector<u8> imageTextureData, u16 imageWidth, u16 imageHeight);
    void mouseDraw(u16 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape);

private:
    void simulate(AppState& state);

    void l_bottomUpUpdate	(); // cancer reading camel case
    void r_bottomUpUpdate	(); // cancer reading camel case
    void snakeUpdate		();
    void golUpdate		    ();


    //inline bool queryAbove(u16 x , u16 y) { return !outOfBounds(x, y - 1) && cells[cellIdx(x, y - 1)].matID == 0; }

    inline bool trySwapAbove(u16 x1, u16 y1, u16 x2, u16 y2);
    inline bool querySwapAbove(u16 x1, u16 y1, u16 x2, u16 y2);

    inline void changeMaterial	(u16 x, u16 y, u8 newMaterial);
    inline void swapCells		(u16 x1, u16 y1, u16 x2, u16 y2);
    inline bool trySwap		    (u16 x1, u16 y1, u16 x2, u16 y2);
    inline bool querySwap		(u16 x1, u16 y1, u16 x2, u16 y2);

    void updateChunk		(u16 x, u16 y);
    bool updateCell			(u16 x, u16 y);
    bool updateSand			(u16 x, u16 y);
    bool updateWater		(u16 x, u16 y);
    bool updateNaturalGas	(u16 x, u16 y);
    
    void updateTextureData(std::vector<u8>& textureData);
    void updateEntireTextureData(std::vector<u8>& textureData);

    // odd guy out aww. I'm sorry
    void createDrawIndicators(u16 x, u16 y, u16 size, u8 shape);
    
    // oh that is satisfying
    void drawCircle			(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawCircleOutline	(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawLine			(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawSquare			(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);
    void drawSquareOutline	(u16 x, u16 y, u16 size, u8 material, u8 drawChance, std::function<void(u16, u16, u8)> foo);

    bool outOfBounds(u16 x, u16 y) { return x >= cellWidth || y >= cellHeight || x < 0 || y < 0; }
    u32 cellIdx		(u16 x, u16 y) { return (y * cellWidth) + x; }	
    u32 textureIdx	(u16 x, u16 y) { return 4 * ((y * textureWidth) + x); }
    //u32 roundUp		(u16 x, u16 y) { return (y > 0) ? (x+y-1) / y : 0; } // jesos
    //u32 chunkIdx	(u16 x, u16 y) { return (roundUp(x, CHUNK_SIZE) * roundUp(y, CHUNK_SIZE)); }
    //void resetChunks()			   { chunks.clear(); chunks.resize(chunkIdx(cellWidth, cellHeight), true); }

    template<typename T> // cheeky template
    inline T getRand(T min = -1, T max = 1) { return splitMix64_NextRand() % (max - min + 1) + min; }
    inline u64 splitMix64_NextRand() {
        u64 z = (seed += UINT64_C(0x9E3779B97F4A7C15));
        z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
        z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
        return z ^ (z >> 31);
    }


    /*----------------------------------------------------------------
    ---- Variables ---------------------------------------------------
    ----------------------------------------------------------------*/
    

    bool sizeChanged = false;

    u8 gasDispersionFactor;
    u8 fluidDispersionFactor;
    u8 solidDispersionFactor;

    u8 nVariants;		
    u8 scaleFactor;
    u16 textureWidth, textureHeight;
    u16 cellWidth, cellHeight;
    u16 chunksWidth, chunksHeight; // this is extremely confusing..
    u64 seed = 1234567890987654321;
    
    //std::vector<bool> chunks; // convert chunks to bitflags
    std::vector<Cell> cells;
    std::vector<Material> materials;
    std::vector<std::pair<u16, u16>> textureChanges;
    std::vector<std::pair<u16, u16>> drawIndicators;
    //std::vector<std::pair<u16, u16>> chunkBorders;
};
