#pragma once
#include "pch.h"


// add:
// >> default dispersion
// >> default temperature
// >> default flammability
// >> default phase (solid, liquid, gas, (plasma???))
struct Material {
    u8                           r, g, b, a;
    u8                           phase;
    u16                          density;
    std::vector<std::vector<u8>> variants;

    Material(u8 RED, u8 GREEN, u8 BLUE, u8 ALPHA, u8 PHASE, u16 DENSITY) {
        r        = RED;
        g        = GREEN;
        b        = BLUE;
        a        = ALPHA;
        phase    = PHASE;
        density  = DENSITY;
        variants = {{RED, GREEN, BLUE, ALPHA}};
    }
    Material() = default;
};

struct Cell {     // 32 bits of data, for more cache hits === speed.
    bool updated; // uses 1 byte??? should just be a bit
    u8   matID;
    u8   variant; // index to array of randomly generated RGBA values from material's RGBA.
    u8   data;    // extra data if needed, e.g fire temp

    Cell(bool UPDATED, u8 MATERIAL, u8 COLOUR_VARIANT, u8 EXTRA_DATA) {
        updated = UPDATED;
        matID   = MATERIAL;
        variant = COLOUR_VARIANT;
        data    = EXTRA_DATA;
    }
    Cell() = default;
};
