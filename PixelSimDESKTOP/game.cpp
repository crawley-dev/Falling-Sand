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
    //if (sizeChanged)
    //    updateEntireTexture(textureData);
    //else
    //    updateTexture(textureData);
}

void Game::reload(u16 width, u16 height, u8 scale) {
    printf("reloaded! width: %d  height: %d  scale: %d", width, height, scale);
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

    auto chunkCoords = toChunkCoords(cameraX, cameraY);
    s16  startChunkX = chunkCoords.first;
    s16  startChunkY = chunkCoords.second;

    s16 iterChunksX = cellWidth / CHUNK_SIZE + func123(cellWidth);
    s16 iterChunksY = cellHeight / CHUNK_SIZE + func123(cellHeight);
    //printf("top left: (%d,%d)\n", startChunkX, startChunkY);

    // scuffed chunk renderer..
    s32 offX = abs(cameraX) % 16;
    s32 offY = abs(cameraY) % 16;
    drawLine(offX, 0, offX, textureHeight - 1);
    drawLine(0, offY, textureWidth - 1, offY);
    for (s32 x = 1; x < iterChunksX; x++) {
        drawLine(offX + (x * CHUNK_SIZE) - 1, 0, offX + (x * CHUNK_SIZE) - 1, textureHeight - 1);
    }
    for (s32 y = 1; y < iterChunksY; y++) {
        drawLine(0, offY + (y * CHUNK_SIZE) - 1, textureWidth - 1, offY + (y * CHUNK_SIZE) - 1);
    }

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
---- Drawing Algorithms ----------------------------------------------------------------
--------------------------------------------------------------------------------------*/


//https://stackoverflow.com/questions/10060046/drawing-lines-with-bresenhams-line-algorithm
void Game::drawLine(s32 x1, s32 y1, s32 x2, s32 y2) {
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx  = x2 - x1;
    dy  = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    px  = 2 * dy1 - dx1;
    py  = 2 * dx1 - dy1;
    if (dy1 <= dx1) {
        if (dx >= 0) {
            x  = x1;
            y  = y1;
            xe = x2;
        } else {
            x  = x2;
            y  = y2;
            xe = x1;
        }
        textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
        //putpixel(x, y, c);
        for (i = 0; x < xe; i++) {
            x = x + 1;
            if (px < 0) {
                px = px + 2 * dy1;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                } else {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
        }
    } else {
        if (dy >= 0) {
            x  = x1;
            y  = y1;
            ye = y2;
        } else {
            x  = x2;
            y  = y2;
            ye = y1;
        }
        textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
        for (i = 0; y < ye; i++) {
            y = y + 1;
            if (py <= 0) {
                py = py + 2 * dx1;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                } else {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
        }
    }
}

/*--------------------------------------------------------------------------------------
---- Texture Update Routines ----------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateTexture(std::vector<u8>& textureData) {
    for (auto& [colour, coords] : textureChanges) { // ALL IN CELL COORDS
        const s32 idx        = textureIdx(coords.first, coords.second);
        textureData[idx + 0] = colour[0];
        textureData[idx + 1] = colour[1];
        textureData[idx + 2] = colour[2];
        textureData[idx + 3] = colour[3];
    }
}

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


    for (Chunk* chunk : chunks) {
        const s32 worldX = chunk->x * CHUNK_SIZE - cameraX;
        const s32 worldY = (chunk->y - 1) * CHUNK_SIZE + cameraY;
        if (outOfBounds(worldX, worldY) && outOfBounds(worldX + CHUNK_SIZE - 1, worldY) &&
            outOfBounds(worldX, worldY + CHUNK_SIZE - 1) && outOfBounds(worldX + CHUNK_SIZE - 1, worldY + CHUNK_SIZE - 1)) {
            continue;
        }

        for (s32 y = 0; y < CHUNK_SIZE; y++)
            for (s32 x = 0; x < CHUNK_SIZE; x++) {
                if (outOfBounds(x + worldX, y + worldY)) continue;

                const Cell&            c       = chunk->cells[cellIdx(x, y)];
                const std::vector<u8>& variant = materials[c.matID].variants[c.variant];

                for (s32 tY = 0; tY < scaleFactor; tY++) {
                    for (s32 tX = 0; tX < scaleFactor; tX++) {
                        const u32 idx        = textureIdx(((x + worldX) * scaleFactor) + tX, ((y + worldY) * scaleFactor) + tY);
                        textureData[idx + 0] = variant[0];
                        textureData[idx + 1] = variant[1];
                        textureData[idx + 2] = variant[2];
                        textureData[idx + 3] = variant[3];
                    }
                }
            }
    }

    for (auto& [colour, coords] : textureChanges) { // ALL IN CELL COORDS
                                                    //for (s32 tY = 0; tY < scaleFactor; tY++) {
                                                    //    for (s32 tX = 0; tX < scaleFactor; tX++) {
        //        const s32 idx = textureIdx((coords.first * scaleFactor) + tX, (coords.second * scaleFactor) + tY);
        const s32 idx        = textureIdx(coords.first, coords.second);
        textureData[idx + 0] = colour[0];
        textureData[idx + 1] = colour[1];
        textureData[idx + 2] = colour[2];
        textureData[idx + 3] = colour[3];
        //    }
        //}
    }
    textureChanges.clear();
}

/*
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
    }*/
