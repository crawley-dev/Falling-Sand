#pragma once
#include "pch.h"
#include "game_chunked.h"

void Sim::init(u16 width, u16 height, u8 scale) {
    sizeChanged   = true;
    scaleFactor   = scale;
    textureWidth  = width;
    textureHeight = height;
    cellWidth     = textureWidth / scaleFactor;
    cellHeight    = textureHeight / scaleFactor;

    solidDispersion  = 2;
    liquidDispersion = 4;
    gasDispersion    = 6;

    // Init materials
    materials.clear();
    materials.resize(MaterialID::COUNT); // just so i can [] access, clear code.
    materials[MaterialID::EMPTY] = Material(50, 50, 50, 255, Phase::SOLID, 0);
    materials[MaterialID::SAND] = Material(245, 215, 176, 255, Phase::SOLID, 1600);
    materials[MaterialID::WATER] = Material(20, 20, 255, 125, Phase::SOLID, 997);
    materials[MaterialID::CONCRETE] = Material(200, 200, 200, 255, Phase::SOLID, 2000);
    materials[MaterialID::NATURAL_GAS] = Material(20, 20, 50, 100, Phase::SOLID, 2000); // uhhh.. very heavy gas?
    materials[MaterialID::GOL_ALIVE] = Material(0, 255, 30, 255, Phase::SOLID, 0); // TODO: fix gas movement density relationship

    // create material colour variations
    for (Material& mat : materials) { 
        mat.variants.clear();
        mat.variants.reserve(VARIANT_COUNT);
        for (u8 i = 0; i < VARIANT_COUNT; i++) {
            if (mat.density == 1600 || mat.density == 997) {
                mat.variants.push_back({
                static_cast<u8>(mat.r - getRand<u8>(0, VARIATION)), 
                static_cast<u8>(mat.g - getRand<u8>(0, VARIATION)), 
                static_cast<u8>(mat.b - getRand<u8>(0, VARIATION)), 
                mat.a });
            } else {
                mat.variants.push_back
                ({mat.r, mat.g, mat.b, mat.a });
            }
        }
    }
}

void Sim::update(AppState& state, std::vector<u8>& textureData) {
    
    if (state.runSim) simulate(state);

    if (sizeChanged) {
        updateEntireTexture(textureData);
        sizeChanged = false;
    } else updateTexture(textureData);
}

void Sim::reload(u16 width, u16 height, u8 scale) {

}

void Sim::reset() {

}

/*--------------------------------------------------------------------------------------
---- Simulation Update Routines --------------------------------------------------------
--------------------------------------------------------------------------------------*/

/*
First Implementation :
    - get the chunk at the camera position
    - calculate how many chunks are visible in x & y ((textureWidth / scaleFactor) / CHUNK_SIZE)
    - loop through from camera Chunk to Final Chunk
        - get chunk from map
        - update chunk's cells bot -> top
*/

void Sim::simulate(AppState& state) {
    s32 cameraChunkX = cameraX / CHUNK_SIZE + (cameraX % CHUNK_SIZE != 0) ? 1 : 0;
    s32 cameraChunkY = cameraY / CHUNK_SIZE + (cameraY % CHUNK_SIZE != 0) ? 1 : 0;

    // precompute? if (!sizeChanged) this is consistent across frames?
    s32 chunksToRenderX = cellWidth / CHUNK_SIZE + (cellWidth % CHUNK_SIZE != 0) ? 1 : 0;
    s32 chunksToRenderY = cellHeight / CHUNK_SIZE + (cellHeight % CHUNK_SIZE != 0) ? 1 : 0;

    for (s32 x = cameraChunkX; x < cameraChunkX + chunksToRenderX; x++) {
        for (s32 y = cameraChunkY; y < cameraChunkY + chunksToRenderY; y++) {
            if (!chunkMap.contains({ x,y })) continue;
            Chunk& chunk = chunkMap[{x, y}];
            if (chunk.updated) continue;
            for (Cell& cell : chunk.cells) {
                updateCell(cell);
            }
        }
    }
}

Chunk* Sim::getChunk(s32 x, s32 y) {
    // if chunk exists, return it
    // else create it and return it

    return nullptr;
}


