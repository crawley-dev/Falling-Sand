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
    cameraX = state.camera.x;
    cameraY = state.camera.y;

    if (state.runSim) simulate(state);
    updateEntireTexture(textureData);

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
    chunks.clear();
    chunkMap.clear();
}

void Game::loadImage(std::vector<u8>& textureData, u16 width, u16 height) {
}

void Game::mouseDraw(AppState& state, s32 x, s32 y, u16 size, u8 drawChance, u8 material, u8 shape) {
    printf("%d,%d\n", x, y);
    x = x / scaleFactor;
    y = y / scaleFactor;

    printf("%d,%d\n\n", x, y);

    if (outOfViewportBounds(x, y)) return;
    auto [mX, mY] = mouseToWorld(x, y);

    auto lambda = [&](s32 _x, s32 _y) -> void { changeMaterial(_x, _y, material); };
    drawCircle(mX, mY, size, material, drawChance, lambda);
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
    static Chunk* mouseChunk = getChunk(0, 0);

    auto func123 = [](s32 length) -> u8 { return (length % CHUNK_SIZE == 0) ? 0 : 1; };

    auto [startChunkX, startChunkY] = worldToChunk(cameraX, cameraY);
    s16 iterChunksX                 = cellWidth / CHUNK_SIZE + func123(cellWidth);
    s16 iterChunksY                 = cellHeight / CHUNK_SIZE + func123(cellHeight);

    //printf("(%d,%d) -> (%d,%d)\n", startChunkX, startChunkY, startChunkX + iterChunksX, startChunkX + iterChunksY);
    //printf("\r                                                    \r"); // return to start of line, print spaces, return again

    for (s16 y = startChunkY; y < startChunkY + iterChunksY; y++)
        for (s16 x = startChunkX; x < startChunkX + iterChunksX; x++) {
            Chunk* chunk = getChunk(x, y);

            //updatedChunks.push_back(chunk);
            //for (u8 cY = 0; cY < CHUNK_SIZE; cY++) {
            //    for (u8 cX = 0; cX < CHUNK_SIZE; cX++) {
            //        // update cell
            //    }
            //}
        }

    if (mouseChunk->cells.size() > CHUNK_SIZE * CHUNK_SIZE || mouseChunk->cells.size() < 0) { mouseChunk = getChunk(0, 0); }
    for (auto& cell : mouseChunk->cells) {
        cell.matID = MaterialID::EMPTY;
    }

    if (outOfTextureBounds(state.mouse.x, state.mouse.y)) { state.mouse = Coord<u16>(0, 0); }

    Coord mouseCell               = Coord<u16>(state.mouse.x, state.mouse.y) / Coord<u16>(scaleFactor);
    const auto [wX, wY]           = mouseToWorld(mouseCell.x, mouseCell.y);
    const auto [mChunkX, mChunkY] = worldToChunk(wX, wY);
    const auto [cX, cY]           = chunkToCell(wX, wY, mChunkX, mChunkY);

    mouseChunk = getChunk(mChunkX, mChunkY);
    //for (auto& cell : mouseChunk->cells) {
    //    cell.matID = MaterialID::GOL_ALIVE;
    //}
    changeMaterial(wX, wY, MaterialID::SAND);

    state.print_hash  = boost::hash<std::pair<s16, s16>>()(std::make_pair(mChunkX, mChunkY));
    state.print_mouse = {mouseCell.x, mouseCell.y};
    state.print_chunk = {mChunkX, mChunkY};
    state.print_cell  = {cX, cY};
}

Chunk* Game::getChunk(s16 x, s16 y) {
    if (chunkMap.contains({x, y})) return chunkMap[{x, y}];
    return createChunk(x, y);
}

Chunk* Game::createChunk(s16 x, s16 y, u8 material) {
    Chunk* chunk = new Chunk(x, y, material);
    chunk->cells.resize(CHUNK_SIZE * CHUNK_SIZE, Cell(MaterialID::EMPTY, getRand<u8>(0, VARIANT_COUNT - 1)));
    chunkMap[{x, y}] = chunk;
    chunks.push_back(chunk);

    printf("creating chunk: (%d,%d) | size: %d\n", x, y, chunks.size());
    return chunk;
}

// shoud be very slow ..
void Game::changeMaterial(s32 x, s32 y, u8 newMaterial) {
    if (outOfViewportBounds(x, y)) {
        printf("nuh uh %d,%d\n", x, y);
        return;
    }

    auto [chunkX, chunkY] = worldToChunk(x, y);
    auto [cellX, cellY]   = chunkToCell(x, y, chunkX, chunkY);
    Chunk* chunk          = getChunk(chunkX, chunkY);
    Cell&  c              = chunk->cells[cellIdx(cellX, cellY)];
    c.matID               = newMaterial;
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
void Game::drawLine(s32 x1, s32 y1, s32 x2, s32 y2, std::function<void(s32, s32)>& foo) {
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx  = x2 - x1;
    dy  = y2 - y1;
    dx1 = fabs(dx); // floating point abs???
    dy1 = fabs(dy); // floating point abs???
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
        foo(x, y);

        //textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
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
            foo(x, y);

            //textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
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
        foo(x, y);

        //textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
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
            foo(x, y);
            //textureChanges.push_back({{255, 30, 30, 255}, {x, y}});
        }
    }
}

void Game::drawCircle(s32 x, s32 y, u16 size, u8 material, u8 drawChance, std::function<void(s32, s32)> foo) {
    int r2   = size * size;
    int area = r2 << 2;
    int rr   = size << 1;

    for (s32 i = 0; i < area; i++) {
        int tX = (i % rr) - size;
        int tY = (i / rr) - size;

        if (tX * tX + tY * tY <= r2)
            if (getRand<s64>(1, 100) <= drawChance) foo(x + tX, y + tY);
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
// not relative to the camera at all?? << i am doing "viewportToWorld" afterall?
void Game::updateEntireTexture(std::vector<u8>& textureData) {

    // beyond ~5 chunks in vertical, it doesn't render anything new
    // might be an outOfViewportBounds error?

    for (u32 y = 0; y < textureHeight; y++)
        for (u32 x = 0; x < textureWidth; x++) {
            const u32 idx        = textureIdx(x, y);
            textureData[idx + 0] = 255;
            textureData[idx + 1] = 255;
            textureData[idx + 2] = 255;
            textureData[idx + 3] = 255;
        }

    for (Chunk* chunk : chunks) {
        const auto [worldX, worldY] = chunkToWorld(chunk->x, chunk->y, 0, 0);
        const auto [texX, texY]     = viewportToWorld(worldX, worldY); // accounts for camera position entirely.

        //if (outOfViewportBounds(worldX, worldY) && outOfViewportBounds(worldX + CHUNK_SIZE - 1, worldY) &&
        //    outOfViewportBounds(worldX, worldY + CHUNK_SIZE - 1) && outOfViewportBounds(worldX + CHUNK_SIZE - 1, worldY + CHUNK_SIZE - 1)) {
        //    continue;
        //}

        for (s32 y = 0; y < CHUNK_SIZE; y++)
            for (s32 x = 0; x < CHUNK_SIZE; x++) {
                if (outOfCellBounds(x + texX, y + texY)) continue; // performance aint even that bad : )

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
                    if (!outOfViewportBounds(texX + i, texY)) {
                        const u32 idx        = textureIdx((i + texX) * scaleFactor + tX, texY * scaleFactor + tY);
                        textureData[idx + 0] = 255;
                        textureData[idx + 1] = 30;
                        textureData[idx + 2] = 30;
                        textureData[idx + 3] = 255;
                    }

                    if (!outOfViewportBounds(texX, texY + i)) {
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

// this function shall for ever be confusiing as shit
// doesn't actually take into account the camera position. thats your job.
// whoever wrote this, certificable spac

bool Game::outOfCellBounds(s32 x, s32 y) const {
    return x >= cellWidth || y >= cellHeight || x < 0 || y < 0;
}

bool Game::outOfViewportBounds(s32 x, s32 y) const {
    return x >= cellWidth + cameraX || y >= cellHeight + cameraY || x < cameraX || y < cameraY;
}

bool Game::outOfChunkBounds(u8 x, u8 y) const {
    return x >= CHUNK_SIZE || y >= CHUNK_SIZE || x < 0 || y < 0;
}

bool Game::outOfTextureBounds(u32 x, u32 y) const {
    return x >= textureWidth || y >= textureHeight || x < 0 || y < 0;
}

std::pair<s16, s16> Game::worldToChunk(s32 x, s32 y) const {
    return {floor(float(x) / CHUNK_SIZE), floor(float(y) / CHUNK_SIZE)};
}

std::pair<s32, s32> Game::chunkToWorld(s16 cX, s16 cY, u8 lX, u8 lY) const {
    return {(cX * CHUNK_SIZE) + lX, (cY * CHUNK_SIZE) + lY};
}

std::pair<s32, s32> Game::viewportToWorld(s32 x, s32 y) const {
    return {x - cameraX, y + cameraY};
}

std::pair<s32, s32> Game::mouseToWorld(s32 x, s32 y) const {
    return {x + cameraX, y - cameraY};
}


std::pair<u8, u8> Game::worldToCell(s32 x, s32 y) const {
    auto [chunkX, chunkY] = worldToChunk(x, y);
    return {x - (chunkX * CHUNK_SIZE), y - (chunkY * CHUNK_SIZE)};
}

std::pair<u8, u8> Game::chunkToCell(s32 x, s32 y, s16 cX, s16 cY) const {
    return {x - (cX * CHUNK_SIZE), y - (cY * CHUNK_SIZE)};
}

u8 Game::cellIdx(u8 x, u8 y) const {
    return (y * CHUNK_SIZE) + x;
}

u32 Game::textureIdx(u16 x, u16 y) const {
    return 4 * ((y * textureWidth) + x);
}

template <typename T> // cheeky template
T Game::getRand(T min, T max) {
    return splitMix64_NextRand() % (max - min + 1) + min;
}

u64 Game::splitMix64_NextRand() {
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
        if (outOfViewportBounds(startX, startY) && outOfViewportBounds(startX + CHUNK_SIZE, startY) && outOfViewportBounds(startX, startY + CHUNK_SIZE) &&
            outOfViewportBounds(startX + CHUNK_SIZE, startY + CHUNK_SIZE))
            continue;

        u8 yOffset{CHUNK_SIZE}, xOffset{CHUNK_SIZE};
        for (s32 y = startY; y < startY + yOffset; y++) {
            for (s32 x = startX; x < startX + xOffset; x++) {
                if (outOfViewportBounds(x, y)) continue;

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
