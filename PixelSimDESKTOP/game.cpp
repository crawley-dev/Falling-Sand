#pragma once
// clang-format off
#include "pch.h"
#include "game.h"
// clang-format on

Game::Game() {
}
Game::~Game() {
    for (Chunk* chunk : chunks) {
        delete chunk;
    }
}

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

    createChunk(0, 0);
    createChunk(2, 0);
    createChunk(4, 0);
    createChunk(-1, -1);
}

void Game::update(AppState& state, std::vector<u8>& textureData) {
    cameraX = state.cameraX;
    cameraY = state.cameraY;

    //drawLine(0, textureHeight / 2, textureWidth - 1, textureHeight / 2);
    //drawLine(textureWidth / 2, 0, textureWidth / 2, textureHeight - 1);


    if (state.runSim) simulate(state);
    updateEntireTexture(textureData);
    //updatedChunks.clear();
    textureChanges.clear();
}

void Game::reload(u16 width, u16 height, u8 scale) {
    printf("reloaded! width: %d  height: %d  scale: %d\n", width, height, scale);
    textureWidth  = width;
    textureHeight = height;
    scaleFactor   = scale;
    cellHeight    = textureHeight / scaleFactor;
    cellWidth     = textureWidth / scaleFactor;
    sizeChanged   = true;
}

void Game::reset() {
    for (auto& chunk : chunks) {
        delete chunk;
    }
    // can probably "re-initialise" these in a simpler way..
    chunks   = std::vector<Chunk*>();
    chunkMap = std::unordered_map<std::pair<s16, s16>, Chunk*, boost::hash<std::pair<s16, s16>>>();
}

void Game::loadImage(std::vector<u8>& textureData, u16 width, u16 height) {
}

void Game::mouseDraw(s32 x, u16 y, u16 size, u8 drawChance, u8 material, u8 shape) {
}

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
    static Chunk* mouseChunk = getChunk(0, 0);

    auto func123 = [](s32 length) -> u8 { return (length % CHUNK_SIZE == 0) ? 0 : 1; };

    auto [startChunkX, startChunkY] = toChunkCoords(cameraX, cameraY);
    s16 iterChunksX                 = cellWidth / CHUNK_SIZE + func123(cellWidth);
    s16 iterChunksY                 = cellHeight / CHUNK_SIZE + func123(cellHeight);
    for (s16 y = startChunkY; y < startChunkY + iterChunksY; y++)
        for (s16 x = startChunkX; x < startChunkX + iterChunksX; x++) {
            Chunk* chunk = getChunk(x, y);
            // ^^ gets or creates a chunk

            //updatedChunks.push_back(chunk);
            //for (u8 cY = 0; cY < CHUNK_SIZE; cY++) {
            //    for (u8 cX = 0; cX < CHUNK_SIZE; cX++) {
            //        // update cell
            //    }
            //}
        }


    // fuck
    if (mouseChunk == nullptr) mouseChunk = getChunk(0, 0);
    for (auto& cell : mouseChunk->cells) {
        cell.matID = MaterialID::EMPTY;
    }

    if (outOfTextureBounds(state.mouseX, state.mouseY)) return;
    s32 mXTex               = state.mouseX / scaleFactor;
    s32 mYTex               = state.mouseY / scaleFactor;
    const auto [mX, mY]     = std::make_pair(mXTex + cameraX, mYTex - cameraY); // inverse of "removeCameraPos", idk why tbh..
    auto [mChunkX, mChunkY] = toChunkCoords(mX, mY);
    //printf("mChunk: (%d,%d)\n", mChunkX, mChunkY);
    mouseChunk = getChunk(mChunkX, mChunkY);
    for (auto& cell : mouseChunk->cells) {
        cell.matID = MaterialID::GOL_ALIVE;
    }

    state.hash    = boost::hash<std::pair<s16, s16>>()(std::make_pair(mChunkX, mChunkY));
    state.mChunkX = mChunkX;
    state.mChunkY = mChunkY;
}

inline Chunk* Game::getChunk(s16 x, s16 y) {
    if (chunkMap.contains({x, y})) return chunkMap[{x, y}];
    return createChunk(x, y);
}

inline Chunk* Game::createChunk(s16 x, s16 y, u8 material) {
    printf("creating chunk: %d, %d\n", x, y);
    Chunk* chunk = new Chunk(x, y, material);
    chunk->cells.resize(CHUNK_SIZE * CHUNK_SIZE,
                        Cell(getRand<u8>(0, MaterialID::COUNT - 1), getRand<u8>(0, VARIANT_COUNT - 1))); // very slow ?
    //for (s32 n = 0; n < 16 * 16; n++) {
    //    chunk->cells[n].matID = getRand<u8>(0, MaterialID::COUNT - 1);
    //}
    chunkMap[{x, y}] = chunk;
    chunks.push_back(chunk);
    return chunk;
}

/*--------------------------------------------------------------------------------------
---- Updating Cells --------------------------------------------------------------------
--------------------------------------------------------------------------------------*/

// aint got to the simulation part yet brother !

/*--------------------------------------------------------------------------------------
---- Drawing Algorithms ----------------------------------------------------------------
--------------------------------------------------------------------------------------*/


// Bresenham's Line Algorithm
// currently pushes changes to textureBuffer.
// currently very slow, try optimised version on this stackoverflow below..
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

// Rendering Pipeline is wrong.. x-axis slips ??
// renders perfectly work camera (0,0)..
// otherwise it shits itself ?

// Seems to be rendering chunks where they would be ABSOLUTE
// not relative to the camera at all?? << i am doing "removeCameraPos" afterall?
void Game::updateEntireTexture(std::vector<u8>& textureData) {

    // beyond ~5 chunks in vertical, it doesn't render anything new
    // might be an outOfBounds error?

    for (u32 y = 0; y < textureHeight; y++)
        for (u32 x = 0; x < textureWidth; x++) {
            const u32 idx        = textureIdx(x, y);
            textureData[idx + 0] = 255;
            textureData[idx + 1] = 255;
            textureData[idx + 2] = 255;
            textureData[idx + 3] = 255;
        }

    for (Chunk* chunk : chunks) {
        //const auto [texX, texY] = toWorldCoords(chunk->x, chunk->y, 0, 0);
        const auto [worldX, worldY] = toWorldCoords(chunk->x, chunk->y, 0, 0);
        const auto [texX, texY]     = removeCameraPos(worldX, worldY); // accounts for camera position entirely.

        //if (outOfBounds(worldX, worldY) && outOfBounds(worldX + CHUNK_SIZE - 1, worldY) &&
        //    outOfBounds(worldX, worldY + CHUNK_SIZE - 1) && outOfBounds(worldX + CHUNK_SIZE - 1, worldY + CHUNK_SIZE - 1)) {
        //    continue;
        //}

        for (s32 y = 0; y < CHUNK_SIZE; y++)
            for (s32 x = 0; x < CHUNK_SIZE; x++) {
                if (outOfBounds(x + texX, y + texY)) continue; // performance aint even that bad : )

                const Cell&            c       = chunk->cells[cellIdx(x, y)];
                const std::vector<u8>& variant = materials[c.matID].variants[c.variant];

                for (s32 tY = 0; tY < scaleFactor; tY++) {
                    for (s32 tX = 0; tX < scaleFactor; tX++) {
                        const u32 idx        = textureIdx(((x + texX) * scaleFactor) + tX, ((y + texY) * scaleFactor) + tY);
                        textureData[idx + 0] = variant[0];
                        textureData[idx + 1] = variant[1];
                        textureData[idx + 2] = variant[2];
                        textureData[idx + 3] = variant[3];
                    }
                }
            }


        for (auto& [colour, coords] : textureChanges) { // ALL IN CELL COORDS
            const s32 idx        = textureIdx(coords.first, coords.second);
            textureData[idx + 0] = colour[0];
            textureData[idx + 1] = colour[1];
            textureData[idx + 2] = colour[2];
            textureData[idx + 3] = colour[3];
        }

        /*
        // TL --> TR
        for (s32 tY = 0; tY < scaleFactor; tY++)
            for (s32 tX = 0; tX < scaleFactor; tX++)
                for (u8 i = 0; i < CHUNK_SIZE; i++) { // draws
                    if (!outOfBounds(texX + i, texY)) {
                        const u32 idx        = textureIdx((i + texX) * scaleFactor + tX, texY * scaleFactor + tY);
                        textureData[idx + 0] = 255;
                        textureData[idx + 1] = 30;
                        textureData[idx + 2] = 30;
                        textureData[idx + 3] = 255;
                    }

                    if (!outOfBounds(texX, texY + i)) {
                        const u32 idx        = textureIdx(texX * scaleFactor + tX, (texY + i) * scaleFactor + tY);
                        textureData[idx + 0] = 255;
                        textureData[idx + 1] = 30;
                        textureData[idx + 2] = 30;
                        textureData[idx + 3] = 255;
                    }
                }*/
    }
}


/*--------------------------------------------------------------------------------------
---- Simple, Inlined Algorithms --------------------------------------------------------
--------------------------------------------------------------------------------------*/

inline bool Game::outOfBounds(s32 x, s32 y) const {
    return x >= cellWidth || y >= cellHeight || x < 0 || y < 0;
}

inline bool Game::outOfTextureBounds(u32 x, u32 y) const {
    return x >= textureWidth || y >= textureHeight || x < 0 || y < 0;
}

inline bool Game::outOfChunkBounds(u8 x, u8 y) const {
    return x >= CHUNK_SIZE || y >= CHUNK_SIZE || x < 0 || y < 0;
}

inline std::pair<s32, s32> Game::removeCameraPos(s32 x, s32 y) const {
    return {x - cameraX, y + cameraY};
}

inline std::pair<s32, s32> Game::toWorldCoords(s16 cX, s16 cY, u8 lX, u8 lY) const {
    return {(cX * CHUNK_SIZE) + lX, (cY * CHUNK_SIZE) + lY};
}

inline std::pair<s16, s16> Game::toChunkCoords(s32 x, s32 y) const {
    return {floor(float(x) / CHUNK_SIZE), floor(float(y) / CHUNK_SIZE)};
}

inline u8 Game::cellIdx(u8 x, u8 y) const {
    return (y * CHUNK_SIZE) + x;
}

inline u32 Game::textureIdx(u16 x, u16 y) const {
    return 4 * ((y * textureWidth) + x);
}

template <typename T> // cheeky template
inline T Game::getRand(T min, T max) {
    return splitMix64_NextRand() % (max - min + 1) + min;
}

inline u64 Game::splitMix64_NextRand() {
    u64 z = (randSeed += UINT64_C(0x9E3779B97F4A7C15));
    z     = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z     = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
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
