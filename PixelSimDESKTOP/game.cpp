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
    //Chunk* chunk1 = new Chunk(0 * CHUNK_SIZE, 0 * CHUNK_SIZE);
    //Chunk* chunk2 = new Chunk(3 * CHUNK_SIZE, 3 * CHUNK_SIZE);
    //Chunk* chunk3 = new Chunk(-1 * CHUNK_SIZE, 0 * CHUNK_SIZE);
    createChunk(0, 0);
    createChunk(2, 0);
    createChunk(4, 0);
    createChunk(-1, -1);
}

void Game::update(AppState& state, std::vector<u8>& textureData) {
    cameraX = state.cameraX;
    cameraY = state.cameraY;

    if (state.runSim) simulate(state);
    updateEntireTexture(textureData);
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
    // iterate in chunks
    auto func123 = [](s32 length) -> u8 { return (length % CHUNK_SIZE == 0) ? 0 : 1; };

    s16 startChunkX, startChunkY;
    if (cameraX < 0 || cameraY < 0) {
        startChunkX = cameraX / CHUNK_SIZE - 1;
        startChunkY = cameraY / CHUNK_SIZE - 1;
    } else {
        startChunkX = cameraX / CHUNK_SIZE;
        startChunkY = cameraY / CHUNK_SIZE;
    }


    s16 iterChunksX = cellWidth / CHUNK_SIZE + func123(cellWidth);
    s16 iterChunksY = cellHeight / CHUNK_SIZE + func123(cellHeight);
    printf("iterating Chunks: (%d,%d) --> (%d,%d)\n", startChunkX, startChunkX, iterChunksX, iterChunksY);

    //for (s32 y = cameraY; y < cellHeight + cameraY; y += CHUNK_SIZE) {
    //    for (s32 x = cameraX; x < cellWidth + cameraX; x += CHUNK_SIZE) {
    //        Chunk* chunk = getChunk(x, y);
    //        for (u8 cY = 0; cY < CHUNK_SIZE; cY++) {
    //            for (u8 cX = 0; cX < CHUNK_SIZE; cX++) {
    //                // update cell
    //            }
    //        }
    //    }
    //}
}

inline Chunk* Game::getChunk(s16 x, s16 y) {
    if (chunkMap.contains({x, y})) return chunkMap[{x, y}];
    return createChunk(x, y);
}

inline Chunk* Game::createChunk(s16 x, s16 y) {
    printf("creating chunk: %d, %d\n", x, y);
    Chunk* chunk = new Chunk(x, y);
    //for (u8 x = 0; x < CHUNK_SIZE * CHUNK_SIZE; x++) {
    //    chunk->cells.emplace_back(MaterialID::SAND, getRand<u8>(0, 10));
    //}
    chunkMap[{x, y}] = chunk;
    chunks.push_back(chunk);
    return chunk;
}

/*--------------------------------------------------------------------------------------
---- Updating Cells --------------------------------------------------------------------
--------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------
---- Texture Update Routines ----------------------------------------------------------
--------------------------------------------------------------------------------------*/


void Game::updateTexture(std::vector<u8>& textureData) {}

void Game::updateEntireTexture(std::vector<u8>& textureData) {
    for (s32 y = 0; y < cellHeight; y++)
        for (s32 x = 0; x < cellWidth; x++) {
            u8 scaler = 1;
            u8 r{44}, g{44}, b{44}, a{255};

            for (s32 tY = 0; tY < scaleFactor / scaler; tY++)
                for (s32 tX = 0; tX < scaleFactor / scaler; tX++) {
                    const s32 idx        = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
                    textureData[idx + 0] = r;
                    textureData[idx + 1] = g;
                    textureData[idx + 2] = b;
                    textureData[idx + 3] = a;
                }
        }

    for (auto& chunk : chunks) { // iter over each chunk
        // next stage:: calc how much of a chunk can be rendered & do up to that.
        // 1100 - (1096 + 8) = -4   << camera(0,0)
        // 1100 - (-16 + 8) = 1108  << camera(0,0)
        //const s32 startY = chunk->y + cameraY;
        //const s32 startX = chunk->x + cameraX;
        //u8 xOffset{0}, yOffset{0};
        //if ((startX + CHUNK_SIZE) < 0 || (startX + CHUNK_SIZE) >= cellWidth)

        const s32 startX = chunk->x * CHUNK_SIZE - cameraX;
        const s32 startY = chunk->y * CHUNK_SIZE + cameraY; // fuck textures.
        if (outOfBounds(startX, startY) && outOfBounds(startX + CHUNK_SIZE, startY) && outOfBounds(startX, startY + CHUNK_SIZE) &&
            outOfBounds(startX + CHUNK_SIZE, startY + CHUNK_SIZE))
            continue;

        u8 yOffset{CHUNK_SIZE}, xOffset{CHUNK_SIZE};
        for (s32 y = startY; y < startY + yOffset; y++) {
            for (s32 x = startX; x < startX + xOffset; x++) {
                if (outOfBounds(x, y)) continue;

                const Cell&            c       = chunk->cells[cellIdx(x - startX, y - startY)];
                const std::vector<u8>& variant = materials[c.matID].variants[c.variant];

                for (s32 tY = 0; tY < scaleFactor; tY++) {
                    for (s32 tX = 0; tX < scaleFactor; tX++) {
                        const u32 idx        = textureIdx((x * scaleFactor) + tX, (y * scaleFactor) + tY);
                        textureData[idx + 0] = variant[0];
                        textureData[idx + 1] = variant[1];
                        textureData[idx + 2] = variant[2];
                        textureData[idx + 3] = variant[3];
                    }
                }
            }
        }
    }

    // draw red dot at centre of world.
    if (cameraX > 0 && cameraY > 0 || cameraX + cellWidth < 0 && cameraY + cellHeight < 0) { return; }
    for (s32 tY = 0; tY < scaleFactor; tY++) {
        for (s32 tX = 0; tX < scaleFactor; tX++) {
            const u32 idx        = textureIdx((abs(cameraX) * scaleFactor) + tX, (abs(cameraY) * scaleFactor) + tY);
            textureData[idx + 0] = 255;
            textureData[idx + 1] = 20;
            textureData[idx + 2] = 20;
            textureData[idx + 3] = 255;
        }
    }
}
