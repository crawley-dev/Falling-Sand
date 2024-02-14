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
    materials[MaterialID::EMPTY]       = Material(50, 50, 50, 255, Phase::SOLID, 0);
    materials[MaterialID::SAND]        = Material(245, 215, 176, 255, Phase::SOLID, 1600);
    materials[MaterialID::WATER]       = Material(20, 20, 255, 125, Phase::SOLID, 997);
    materials[MaterialID::CONCRETE]    = Material(200, 200, 200, 255, Phase::SOLID, 2000);
    materials[MaterialID::NATURAL_GAS] = Material(20, 20, 50, 100, Phase::SOLID, 2000);
    materials[MaterialID::GOL_ALIVE]   = Material(0, 255, 30, 255, Phase::SOLID, 0);

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
}

void Game::update(AppState& state, std::vector<u8>& textureData) {

    // if (state.runSim) simulate(state);
    updateEntireTexture(textureData);
    // if (sizeChanged) {
    //     updateEntireTexture(textureData);
    //     sizeChanged = false;
    // } else
    //     updateTexture(textureData);
}

void Game::reload(u16 width, u16 height, u8 scale) {}

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

    // for (s32 y = cameraChunkY; y < cameraChunkY + chunksToRenderY; y++)
    //     for (s32 x = cameraChunkX; x < cameraChunkX + chunksToRenderX; x++) {
    //         if (!chunkMap.contains({ x, y })) createChunk(x, y);
    //         Chunk& chunk = chunkMap[{x, y}];
    //         if (chunk.updated) continue;
    //         for (Cell& cell : chunk.cells) {
    //             updateCell(cell);
    //         }
    //     }
}

Chunk* Game::getChunk(s32 x, s32 y) {
    // if chunk exists, return it
    // else create it and return it

    return nullptr;
}

void Game::createChunk(s32 x, s32 y) {}

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
    }
    */
    for (s32 y = 0; y < textureHeight; y++) {
        for (s32 x = 0; x < textureWidth; x++) {
            const s32 idx        = textureIdx(x, y);
            textureData[idx + 0] = 20;
            textureData[idx + 1] = 255;
            textureData[idx + 2] = 50;
            textureData[idx + 3] = 255;
        }
    }
}
