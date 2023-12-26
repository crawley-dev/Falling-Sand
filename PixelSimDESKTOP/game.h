#pragma once
#include "pch.h"
#include "cell.h"
#include "interfaceData.h"

class Game
{
public:
	Game();
	~Game();

	void init();
	void reload();
	//void loadImage();
	void reset();
	void update(interfaceData& iData);

	inline constexpr u64 splitMix64_NextRand()
	{
		u64 z = (x += UINT64_C(0x9E3779B97F4A7C15));
		z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
		z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
		return z ^ (z >> 31);
	}

	inline constexpr u64 boundedRand(std::vector<u64> range) // slow as shit
	{ return range[splitMix64_NextRand() % range.size()]; }

	inline constexpr u8 randOffset()
	{ return randRange[splitMix64_NextRand() % 3]; } // hard coded. 

private:
	u8 randRange[3] = { -1, 0, 1 };
	u64 x = 1234567890987654321; // Splitmix64 seed.

	u8 cellScale;
	u32 texWidth, texHeight, cellWidth, cellHeight;
	std::vector<Cell> cells;
	std::vector<GLubyte> textureData;
	std::vector<Material> materials;
};
