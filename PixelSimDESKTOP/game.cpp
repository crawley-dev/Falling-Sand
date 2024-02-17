#pragma once
// clang-format off
#include "pch.h"
#include "game.h"
// clang-format on

void Game::init(u16 width, u16 height, u8 scale) {
    sizeChanged   = true;
    scaleFactor   = scale;
    textureWidth  = width;
    textureHeight = height;
    cellWidth     = textureWidth / scaleFactor;
    cellHeight    = textureHeight / scaleFactor;

    printf("textureWidth: %d, textureHeight: %d\n", textureWidth, textureHeight);

    solidDispersion = 2;
    fluidDispersion = 4;
    gasDispersion   = 6;

    // Init materials
    materials.clear();
    materials.resize(MaterialID::COUNT); // just so i can [] access, clear code.
    materials[MaterialID::EMPTY]       = Material(50, 50, 50, 255, 0);
    materials[MaterialID::SAND]        = Material(245, 215, 176, 255, 1600);
    materials[MaterialID::WATER]       = Material(20, 20, 255, 125, 997);
    materials[MaterialID::CONCRETE]    = Material(200, 200, 200, 255, 2000);
    materials[MaterialID::NATURAL_GAS] = Material(20, 20, 50, 100, 2000);
    materials[MaterialID::GOL_ALIVE]   = Material(0, 255, 30, 255, 0);

    // create material colour variations
    for (Material& mat : materials) {
        mat.variants.clear();
        mat.variants.reserve(VARIANT_COUNT);
        for (u8 i = 0; i < VARIANT_COUNT; i++) {
            if (mat.density == 1600 || mat.density == 997) {
                mat.variants.push_back({static_cast<u8>(mat.r - getRand<u8>(0, VARIATION)),
                                        static_cast<u8>(mat.g - getRand<u8>(0, VARIATION)),
                                        static_cast<u8>(mat.b - getRand<u8>(0, VARIATION)),
                                        mat.a});
            } else {
                mat.variants.push_back({mat.r, mat.g, mat.b, mat.a});
            }
        }
    }

    // just gonna heap alloc them
    // lets see how fast they are..
    Chunk* chunk1 = new Chunk(0 * CHUNK_SIZE, 0 * CHUNK_SIZE);
    Chunk* chunk2 = new Chunk(3 * CHUNK_SIZE, 3 * CHUNK_SIZE);
    Chunk* chunk3 = new Chunk(-1 * CHUNK_SIZE, 0 * CHUNK_SIZE);

    chunks.push_back(chunk1);
    chunks.push_back(chunk2);
    chunks.push_back(chunk3);
}

void Game::update(AppState& state, std::vector<u8>& textureData) {
    cameraX = state.cameraX;
    cameraY = state.cameraY;

    //if (state.runSim) simulate(state);
    if (state.runSim) updateEntireTexture(textureData);
    // if (sizeChanged) {
    //     updateEntireTexture(textureData);
    //     sizeChanged = false;
    // } else
    //     updateTexture(textureData);
}

void Game::reload(u16 width, u16 height, u8 scale) {
    printf("reloaded\n width: %d\n  height: %d\n  scale: %d\n", width, height, scale);
    textureWidth  = width;
    textureHeight = height;
    scaleFactor   = scale;
    cellHeight    = textureHeight / scaleFactor;
    cellWidth     = textureWidth / scaleFactor;
    sizeChanged   = true;
}

void Game::reset() {}

void Game::loadImage(std::vector<u8>& textureData, u16 width, u16 height) {}

void Game::mouseDraw(s32 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape) {}

/*--------------------------------------------------------------------------------------
---- Simulation Update Routines --------------------------------------------------------
--------------------------------------------------------------------------------------*/

/*
First Implementation :
    - get the chunk at the camera position
    - calculate how many chunks are visible in x & y ((textureWidth /
scaleFactor) / CHUNK_SIZE)
    - loop through from camera Chunk to Final Chunk
        - get chunk from map
        - update chunk's cells bot -> top
*/

void Game::simulate(AppState& state) {
    s32 cameraChunkX = cameraX / CHUNK_SIZE + (cameraX % CHUNK_SIZE != 0) ? 1 : 0;
    s32 cameraChunkY = cameraY / CHUNK_SIZE + (cameraY % CHUNK_SIZE != 0) ? 1 : 0;

    // precompute? if (!sizeChanged) this is consistent across frames?
    s32 chunksToRenderX = cellWidth / CHUNK_SIZE + (cellWidth % CHUNK_SIZE != 0) ? 1 : 0;
    s32 chunksToRenderY = cellHeight / CHUNK_SIZE + (cellHeight % CHUNK_SIZE != 0) ? 1 : 0;
    printf("camera chunk x: %d, camera chunk y: %d\n", cameraChunkX, cameraChunkY);

    /*
    for (s32 y = cameraChunkY; y < cameraChunkY + chunksToRenderY; y++)
        for (s32 x = cameraChunkX; x < cameraChunkX + chunksToRenderX; x++) {
            //Chunk& chunk = getChunk(x, y);
            //if (chunk.updated) continue;
            // chunkChanges.push_back(chunk); // needs to be changed.
            switch (state.scanMode) {
            case Scan::BOTTOM_UP_L: l_bottomUpUpdate(chunk); break;
            case Scan::BOTTOM_UP_R: r_bottomUpUpdate(chunk); break;
            }
        }
        */
}

void Game::l_bottomUpUpdate(Chunk& chunk) {
    for (s32 y = CHUNK_SIZE - 1; y >= 0; y--)
        for (s32 x = 0; x < CHUNK_SIZE; x++) {
            //if (chunk.cellUpdates & (1 << ((y * CHUNK_SIZE) + x))) {
            updateCell(chunk.cells[cellIdx(x, y)], x + chunk.x, y + chunk.y);
            //}
        }
}

void Game::r_bottomUpUpdate(Chunk& chunk) {
    for (s32 y = CHUNK_SIZE - 1; y >= 0; y--)
        for (s32 x = CHUNK_SIZE - 1; x >= 0; x--) {
            updateCell(chunk.cells[cellIdx(x, y)], x + chunk.x, y + chunk.y);
        }
}

/*
inline Chunk& Game::getChunk(s32 x, s32 y) {
    if (!chunkMap.contains({x, y})) {
        Chunk chunk = Chunk(x, y);
        chunks.push_back(chunk);
        chunkMap[{x, y}] = chunk;
        printf("Chunk (%d,%d) created", x, y);
    }
    Chunk& chunk = chunkMap[{x, y}];
    return chunk;
}
    */

//inline void Game::createChunk(s32 x, s32 y) { chunkMap[{x, y}] = Chunk(x, y); }

/*--------------------------------------------------------------------------------------
---- Updating Cells --------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

bool Game::updateCell(Cell& c, u16 x, u16 y) {
    //if (c.updated) return true;
    switch (c.matID) {
    //case MaterialID::EMPTY:       return false;
    //case MaterialID::CONCRETE:    return false;
    //case MaterialID::SAND: return updateSand(x, y);
    //case MaterialID::WATER:       return updateWater(x, y);
    //case MaterialID::NATURAL_GAS: return updateNaturalGas(x, y);
    default:
        return false;
        //case MaterialID::FIRE:          return updateFire(x, y);
    }
}

/*--------------------------------------------------------------------------------------
---- Texture Update Routines ----------------------------------------------------------
--------------------------------------------------------------------------------------*/


void Game::updateTexture(std::vector<u8>& textureData) {}

void Game::updateEntireTexture(std::vector<u8>& textureData) {
    /*
    for (s32 y = 0; y < cellHeight; y++) {
        for (s32 x = 0; x < cellWidth; x++) {
            // Cell &c                        = cells[cellIdx(x, y)];
            // const std::vector<u8> &variant = materials[c.matID].variants[c.variant];

            // const u8 red   = variant[0];
            // const u8 green = variant[1];
            // const u8 blue  = variant[2];
            // const u8 alpha = variant[3];


            for (s32 tY = 0; tY < scaleFactor; tY++)
                for (s32 tX = 0; tX < scaleFactor; tX++) {
                    const s32 idx        = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
                    textureData[idx + 0] = 20;
                    textureData[idx + 1] = 255;
                    textureData[idx + 2] = 50;
                    textureData[idx + 3] = 255;
                }
            // c.updated = false;
        }
    }*/

    //for (auto& chunk : chunkChanges) {
    //    for (s32 y = 0; y < CHUNK_SIZE; y++)
    //        for (s32 x = 0; x < CHUNK_SIZE; x++) {
    //            const Cell&            c       = chunk.cells[cellIdx(x, y)];
    //            const std::vector<u8>& variant = materials[c.matID].variants[c.variant];
    //
    //            for (s32 tY = 0; tY < scaleFactor; tY++)
    //                for (s32 tX = 0; tX < scaleFactor; tX++) {
    //                    const s32 idx        = textureIdx((x + chunk.x) * scaleFactor + tX, (y + chunk.y) * scaleFactor + tY);
    //                    textureData[idx + 0] = variant[0];
    //                    textureData[idx + 1] = variant[1];
    //                    textureData[idx + 2] = variant[2];
    //                    textureData[idx + 3] = variant[3];
    //                }
    //        }
    //}

    for (s32 y = 0; y < cellHeight; y++)
        for (s32 x = 0; x < cellWidth; x++) {
            u8 scaler = 1;
            u8 r{44}, g{44}, b{44}, a{255};
            //if ((x + cameraX) % CHUNK_SIZE == 0 || (y + cameraY) % CHUNK_SIZE == 0) { // chunk borders
            //    r      = 255;
            //    g      = 20;
            //    b      = 20;
            //    a      = 255;
            //    scaler = 2;
            //}

            for (s32 tY = 0; tY < scaleFactor / scaler; tY++)
                for (s32 tX = 0; tX < scaleFactor / scaler; tX++) {
                    const s32 idx        = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
                    textureData[idx + 0] = r;
                    textureData[idx + 1] = g;
                    textureData[idx + 2] = b;
                    textureData[idx + 3] = a;
                }
        }

    auto calcOffset = [&](s32 x, s32 y) -> u8 {

    };

    for (auto& chunk : chunks) { // iter over each chunk
        // bounds check chunk, if its inside render Texture (camera offset)
        // first: do for each cell
        // next stage:: calc how much of a chunk can be rendered & do up to that.

        // 1100 - (1096 + 8) = -4   << camera(0,0)
        // 1100 - (-16 + 8) = 1108  << camera(0,0)


        if (outofCameraBounds(chunk->x, chunk->y)) continue;
        const s32 startY = chunk->y + cameraY;
        const s32 startX = chunk->x + cameraX;


        for (s32 y = startY; y < startY + yOffset; y++)
            for (s32 x = startX; x < startX + xOffset; x++) {
                //if (x >= cellWidth || y >= cellHeight || x < 0 || y < 0) {
                //    //printf("Cell Out of Bounds: (%d,%d)\n", x, y);
                //    continue;
                //}

                const Cell&            c       = chunk->cells[cellIdx(x - startX, y - startY)];
                const std::vector<u8>& variant = materials[c.matID].variants[c.variant];

                for (s32 tY = 0; tY < scaleFactor; tY++)
                    for (s32 tX = 0; tX < scaleFactor; tX++) {
                        const u32 idx = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);

                        textureData[idx + 0] = variant[0];
                        textureData[idx + 1] = variant[1];
                        textureData[idx + 2] = variant[2];
                        textureData[idx + 3] = variant[3];
                    }
            }
    }
}
