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
    textureSize.x = width;
    textureSize.y = height;
    cellSize.x    = textureSize.x / scaleFactor;
    cellSize.y    = textureSize.y / scaleFactor;

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
    camera = state.camera;

    if (state.runSim) simulate(state);
    updateEntireTexture(textureData);

    textureChanges.clear();
    state.camera = camera;
}

void Game::reload(u16 width, u16 height, u8 scale) {
    printf("reloaded! width: %d  height: %d  scale: %d\n", width, height, scale);
    //textureSize.x  = width;
    //textureSize.y = height;
    //cellSize.y  = textureSize.y / scaleFactor;
    //cellSize.x   = textureSize.x / scaleFactor;
    textureSize = {width, height};
    scaleFactor = scale;
    cellSize    = textureSize / scaleFactor;
    sizeChanged = true;
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

void Game::mouseDraw(AppState& state, u16 mX, u16 mY, u16 size, u8 drawChance, u8 material, u8 shape) {
    mX /= scaleFactor;
    mY /= scaleFactor;


    if (outOfViewportBounds(mX, mY)) {
        Coord<s16> viewport = worldToViewport(mX, mY);
        printf("oob: %d,%d | %d,%d\n", mX, mY, viewport.x, viewport.y);
        return;
    };
    auto [wX, wY] = viewportToWorld(mX, mY);
    printf("%d,%d\n", wX, wY);

    auto [chunkX, chunkY] = worldToChunk(wX, wY);
    auto [cellX, cellY]   = chunkToCell(chunkX, chunkY, 0, 0);
    Chunk* chunk          = getChunk(chunkX, chunkY);
    for (Cell& c : chunk->cells) {
        c.matID = MaterialID::CONCRETE;
    }

    //for (s32 tY = -size / 2; tY < size / 2; tY++)
    //    for (s32 tX = -size / 2; tX < size / 2; tX++) {
    //changeMaterial(x + tX, y + tY, MaterialID::CONCRETE);
    //}

    //auto lambda = [&](s32 _x, s32 _y) -> void { changeMaterial(_x, _y, material); };
    //drawSquare(x, y, size, material, drawChance, lambda);
}

/*--------------------------------------------------------------------------------------
---- Simulation Update Routines --------------------------------------------------------
--------------------------------------------------------------------------------------*/

/*
First Implementation :
    - get the chunk at the camera position
    - calculate how many chunks are visible in x & y ((textureSize.x /
scaleFactor) / CHUNK_SIZE)
    - loop through from camera Chunk to Final Chunk
        - get chunk from map
        - update chunk's cells bot -> top
*/

void Game::simulate(AppState& state) {
    static Chunk* mouseChunk = getChunk(0, 0);

    auto func123 = [](s32 length) -> u8 { return (length % CHUNK_SIZE == 0) ? 0 : 1; };

    auto [startChunkX, startChunkY] = worldToChunk(camera.x, camera.y);
    s16 iterChunksX                 = cellSize.x / CHUNK_SIZE + func123(cellSize.x);
    s16 iterChunksY                 = cellSize.y / CHUNK_SIZE + func123(cellSize.y);

    for (s16 y = startChunkY; y < startChunkY + iterChunksY; y++)
        for (s16 x = startChunkX; x < startChunkX + iterChunksX; x++) {
            Chunk* chunk = getChunk(x, y);
        }

    if (mouseChunk->cells.size() > CHUNK_SIZE * CHUNK_SIZE || mouseChunk->cells.size() < 0) { mouseChunk = getChunk(0, 0); }
    if (outOfTextureBounds(state.mouse.x, state.mouse.y)) { state.mouse = Coord<u16>(0, 0); }


    Coord mouse             = Coord<u16>(state.mouse.x, state.mouse.y) / Coord<u16>(scaleFactor);
    auto [wX, wY]           = viewportToWorld(mouse.x, mouse.y);
    auto [mChunkX, mChunkY] = worldToChunk(wX, wY);
    auto [cX, cY]           = chunkToCell(wX, wY, mChunkX, mChunkY);
    auto [vX, vY]           = worldToViewport(mouse.x, mouse.y);

    //for (auto& cell : mouseChunk->cells) {
    //    cell.matID = MaterialID::EMPTY;
    //}
    mouseChunk = getChunk(mChunkX, mChunkY);
    //for (auto& cell : mouseChunk->cells) {
    //    cell.matID = MaterialID::GOL_ALIVE;
    //}
    //changeMaterial(wX, wY, MaterialID::SAND);

    state.print_mouse    = {mouse.x, mouse.y};
    state.print_viewport = {vX, vY};
    state.print_world    = {wX, wY};
    state.print_chunk    = {mChunkX, mChunkY};
    state.print_cell     = {cX, cY};
}

// if a chunk exists, grab it. else create one in its place.
Chunk* Game::getChunk(s16 x, s16 y) {
    if (chunkMap.contains({x, y})) return chunkMap[{x, y}];
    return createChunk(x, y);
}

// 'new' specifies heap allocation
Chunk* Game::createChunk(s16 x, s16 y, u8 material) {
    Chunk* chunk     = new Chunk(x, y, material);
    chunkMap[{x, y}] = chunk;
    chunks.push_back(chunk);
    return chunk;
}

// shoud be very slow ..
void Game::changeMaterial(s32 x, s32 y, u8 newMaterial) {
    auto [chunkX, chunkY] = worldToChunk(x, y);
    auto [cellX, cellY]   = chunkToCell(x, y, chunkX, chunkY);
    Chunk* chunk          = getChunk(chunkX, chunkY);

    //if (chunkX < -200 || chunkX > 200 || chunkY < -200 || chunkX > 200 || cellX >= CHUNK_SIZE || cellX < 0 || cellY >= CHUNK_SIZE ||
    //    cellY < 0) {
    //    printf("chunk: (%d,%d) (%d,%d)", chunkX, chunkY, cellX, cellY);
    //}

    Cell& c = chunk->cells[cellIdx(cellX, cellY)];
    c.matID = newMaterial;
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
void Game::drawLine(s32 x1, s32 y1, s32 x2, s32 y2, std::function<void(s32, s32)> foo) {
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

void Game::drawSquare(s32 x, s32 y, u16 size, u8 material, u8 drawChance, std::function<void(s32, s32)> foo) {
    for (s32 tY = -size / 2; tY < size / 2; tY++)
        for (s32 tX = -size / 2; tX < size / 2; tX++)
            if (getRand<s64>(1, 100) <= drawChance) foo(x + tX, y + tY);
}


/*--------------------------------------------------------------------------------------
---- Texture Update Routines ----------------------------------------------------------
--------------------------------------------------------------------------------------*/

void Game::updateTexture(std::vector<u8>& textureData) {
    for (auto& [colour, coords] : textureChanges) { // ALL IN CELL COORDS
        s32 idx              = textureIdx(coords.x, coords.y);
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

    for (u32 y = 0; y < textureSize.y; y++) // black background ??
        for (u32 x = 0; x < textureSize.x; x++) {
            u32 idx              = textureIdx(x, y);
            textureData[idx + 0] = 255;
            textureData[idx + 1] = 255;
            textureData[idx + 2] = 255;
            textureData[idx + 3] = 255;
        }


    // lots of "excess" calculations but it works!!
    for (s32 y = 0; y < cellSize.y; y++) {
        for (s32 x = 0; x < cellSize.x; x++) {
            auto [worldX, worldY] = viewportToWorld(x, y);
            auto [chunkX, chunkY] = worldToChunk(worldX, worldY);
            auto [cellX, cellY]   = worldToCell(worldX, worldY);
            Chunk* chunk          = getChunk(chunkX, chunkY);

            Cell&            cell    = chunk->cells[cellIdx(cellX, cellY)];
            std::vector<u8>& variant = materials[cell.matID].variants[cell.variant];

            //loops aren't performant for small scaleFactor values for (u8 dY = 0; dY < scaleFactor; dY++) {
            for (u8 dY = 0; dY < scaleFactor; dY++)
                for (u8 dX = 0; dX < scaleFactor; dX++) {
                    u32 idx              = textureIdx((x * scaleFactor) + dX, (y * scaleFactor) + dY);
                    textureData[idx + 0] = variant[0];
                    textureData[idx + 1] = variant[1];
                    textureData[idx + 2] = variant[2];
                    textureData[idx + 3] = variant[3];
                }
        }
    }

    //dwa123(textureData);

    // dead code, just bad
    //for (Chunk* chunk : chunks) {
    //    const auto [worldX, worldY] = chunkToWorld(chunk->x, chunk->y, 0, 0);
    //    const auto [texX, texY]     = worldToViewport(worldX, worldY);
    //if (outOfViewportBounds(worldX, worldY) && outOfViewportBounds(worldX + CHUNK_SIZE - 1, worldY) &&
    //    outOfViewportBounds(worldX, worldY + CHUNK_SIZE - 1) && outOfViewportBounds(worldX + CHUNK_SIZE - 1, worldY + CHUNK_SIZE - 1)) {
    //    continue;
    //}
    //
    //    for (s32 y = 0; y < CHUNK_SIZE; y++)
    //        for (s32 x = 0; x < CHUNK_SIZE; x++) {
    //            if (outOfCellBounds(x + texX, y + texY)) continue; // performance aint even that bad : )
    //
    //            const Cell&            c       = chunk->cells[cellIdx(x, y)];
    //            const std::vector<u8>& variant = materials[c.matID].variants[c.variant];
    //
    //            for (s32 tY = 0; tY < scaleFactor; tY++) {
    //                for (s32 tX = 0; tX < scaleFactor; tX++) {
    //                    const u32 idx        = textureIdx(((x + texX) * scaleFactor) + tX, ((y + texY) * scaleFactor) + tY);
    //                    textureData[idx + 0] = variant[0];
    //                    textureData[idx + 1] = variant[1];
    //                    textureData[idx + 2] = variant[2];
    //                    textureData[idx + 3] = variant[3];
    //                }
    //            }
    //        }
    //}

    //for (auto& [colour, coords] : textureChanges) { // ALL IN CELL COORDS
    //    const s32 idx        = textureIdx(coords.x, coords.y);
    //    textureData[idx + 0] = colour[0];
    //    textureData[idx + 1] = colour[1];
    //    textureData[idx + 2] = colour[2];
    //    textureData[idx + 3] = colour[3];
    //}
}


// ideally a more optimal solution.
// not working currently.
void Game::dwa123(std::vector<u8>& textureData) {
    auto func123 = [](s32 length) -> u8 { return (length % CHUNK_SIZE == 0) ? 0 : 1; };

    auto [startChunkX, startChunkY] = worldToChunk(camera.x, camera.y);
    s16 iterChunksX                 = cellSize.x / CHUNK_SIZE + func123(cellSize.x);
    s16 iterChunksY                 = cellSize.y / CHUNK_SIZE + func123(cellSize.y);


    // loop chunks in viewport
    //
    for (s16 chunkY = startChunkY; chunkY < iterChunksY; chunkY++)
        for (s16 chunkX = startChunkX; chunkX < iterChunksX; chunkX++) {
            //Chunk* chunk = getChunk(chunkX, chunkY);
            if (!chunkMap.contains({chunkX, chunkY})) continue;
            Chunk* chunk = chunkMap[{chunkX, chunkY}];

            // change start position of iteration if left most // top most chunk
            // change end position of iteration if right most // bottom most chunk
            u8 startX{0}, startY{0}, endX{CHUNK_SIZE}, endY{CHUNK_SIZE};
            if (chunkY == 0) { // start is equal to how far into the chunk the camera is
                // ...
            } else if (chunkY == iterChunksY - 1) { // end is equal to (chnk + 15) - camera
                // ...
            }

            if (chunkX == 0) {
                // ...
            } else if (chunkX == iterChunksX - 1) {
                // ...
            }


            for (u8 clY = startY; clY < endY; clY++)
                for (u8 clX = startX; clX < endX; clX++) {

                    auto [wX, wY] = chunkToWorld(chunkX, chunkY, clX, clY);
                    auto [vX, vY] = worldToViewport(wX, wY);
                    //s32 vX =

                    if (outOfCellBounds(vX, vY)) { continue; }

                    //if (outOfViewportBounds(wX, wY)) {
                    //    printf("oob viewport\n");
                    //    printf("world:    %d,%d\nviewport: %d,%d\n\n", wX, wY, vX, vY);
                    //    continue;
                    //} else if (outOfCellBounds(vX, vY)) {
                    //    printf("oob cell\n");
                    //    printf("world:    %d,%d\nviewport: %d,%d\n\n", wX, wY, vX, vY);
                    //    continue;
                    //}
                    //if (outOfViewportBounds(wX, wY)) continue;

                    Cell&            cell    = chunk->cells[cellIdx(clX, clY)];
                    std::vector<u8>& variant = materials[cell.matID].variants[cell.variant];

                    //loops aren't performant for small scaleFactor values for (u8 dY = 0; dY < scaleFactor; dY++) {
                    for (u8 dY = 0; dY < scaleFactor; dY++)
                        for (u8 dX = 0; dX < scaleFactor; dX++) {
                            u32 idx              = textureIdx((vX * scaleFactor) + dX, (vY * scaleFactor) + dY);
                            textureData[idx + 0] = variant[0];
                            textureData[idx + 1] = variant[1];
                            textureData[idx + 2] = variant[2];
                            textureData[idx + 3] = variant[3];
                        }
                }
        }
}

/*--------------------------------------------------------------------------------------
---- Simple, Inlined Algorithms --------------------------------------------------------
--------------------------------------------------------------------------------------*/


// this function shall for ever be confusiing as shit
// doesn't actually take into account the camera position. thats your job.
// whoever wrote this, certificable spac
bool Game::outOfCellBounds(s32 x, s32 y) const {
    return x >= cellSize.x || y >= cellSize.y || x < 0 || y < 0;
}

bool Game::outOfViewportBounds(s32 x, s32 y) const { // is correct?
    Coord<s16> viewport = worldToViewport(x, y);
    return viewport.x >= cellSize.x || viewport.y >= cellSize.y || viewport.x < 0 || viewport.y < 0;
    //return x >= cellSize.x + camera.x || y >= cellSize.y + camera.y || x < camera.x || y < camera.y;
}

bool Game::outOfChunkBounds(u8 x, u8 y) const {
    return x >= CHUNK_SIZE || y >= CHUNK_SIZE || x < 0 || y < 0;
}

bool Game::outOfTextureBounds(u32 x, u32 y) const {
    return x >= textureSize.x || y >= textureSize.y || x < 0 || y < 0;
}

Coord<s16> Game::worldToChunk(s32 x, s32 y) const {
    return Coord<s16>(floor(float(x) / CHUNK_SIZE), floor(float(y) / CHUNK_SIZE));
}

Coord<s32> Game::chunkToWorld(s16 x, s16 y, u8 clX, u8 clY) const {
    return Coord<s32>((x * CHUNK_SIZE) + clX, (y * CHUNK_SIZE) + clY);
}

// 'meant' to zero out world coordinates to the viewport, i.e camera = (0,0) on texture.
Coord<s16> Game::worldToViewport(s32 x, s32 y) const {
    s32 camX = (camera.x < 0) ? camera.x : camera.x * -1;
    s32 camY = (camera.y < 0) ? camera.y : camera.y * -1;
    return Coord<s16>(x - camera.x, y - camera.y);
}

Coord<s32> Game::viewportToWorld(s16 x, s16 y) const {
    //return Coord<s32>(x + camera.x, y - camera.y);
    return Coord<s32>(x + camera.x, y + camera.y);
}

Coord<u8> Game::worldToCell(s32 x, s32 y) const {
    auto [chunkX, chunkY] = worldToChunk(x, y);
    return Coord<u8>(x - (chunkX * CHUNK_SIZE), y - (chunkY * CHUNK_SIZE));
}

Coord<u8> Game::chunkToCell(s16 x, s16 y, u8 clX, u8 clY) const {
    return Coord<u8>(x - (clX * CHUNK_SIZE), y - (clY * CHUNK_SIZE));
}

u8 Game::cellIdx(u8 x, u8 y) const {
    return (y * CHUNK_SIZE) + x;
}

u32 Game::textureIdx(u16 x, u16 y) const {
    return 4 * ((y * textureSize.x) + x);
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


class TaskQueue {
public:
    TaskQueue(u8 numThreads) : stop(false) {
        for (u8 i = 0; i < numThreads; i++) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template <typename T>
    void enqueueTask(T task) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }

    ~TaskQueue() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& worker : threads)
            worker.join();
    }

private:
    std::vector<std::thread>          threads;
    std::queue<std::function<void()>> tasks;
    std::mutex                        mutex;
    std::condition_variable           condition;
    bool                              stop;
};


/*
 
  //s32  lineY  = textureSize.y / 2;
    //s32  endX   = (textureSize.x > 0) ? textureSize.x - 1 : textureSize.x;
    //auto lambda = [&](s32 _x, s32 _y) -> void { textureChanges.push_back({{20, 255, 20, 255}, {_x, _y}}); };
    //drawLine(0, lineY, endX, lineY, lambda);


        // TL --> TR
//        for (s32 tY = 0; tY < scaleFactor; tY++)
//            for (s32 tX = 0; tX < scaleFactor; tX++)
//                for (u8 i = 0; i < CHUNK_SIZE; i++) { // draws
//                    if (!outOfViewportBounds(texX + i, texY)) {
//                        const u32 idx        = textureIdx((i + texX) * scaleFactor + tX, texY * scaleFactor + tY);
//                        textureData[idx + 0] = 255;
//                        textureData[idx + 1] = 30;
//                        textureData[idx + 2] = 30;
//                        textureData[idx + 3] = 255;
//                    }
//
//                    if (!outOfViewportBounds(texX, texY + i)) {
//                        const u32 idx        = textureIdx(texX * scaleFactor + tX, (texY + i) * scaleFactor + tY);
//                        textureData[idx + 0] = 255;
//                        textureData[idx + 1] = 30;
//                        textureData[idx + 2] = 30;
//                        textureData[idx + 3] = 255;
//                    }
* 
* 
    for (auto& chunk : chunks) { // iter over each chunk
        // next stage:: calc how much of a chunk can be rendered & do up to that.
        // 1100 - (1096 + 8) = -4   << camera(0,0)
        // 1100 - (-16 + 8) = 1108  << camera(0,0)
        //const s32 startY = chunk->y + camera.y;
        //const s32 startX = chunk->x + camera.x;
        //u8 xOffset{0}, yOffset{0};
        //if ((startX + CHUNK_SIZE) < 0 || (startX + CHUNK_SIZE) >= cellSize.x)

        const s32 startX = chunk->x * CHUNK_SIZE - camera.x;
        const s32 startY = chunk->y * CHUNK_SIZE + camera.y; // fuck textures.
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
