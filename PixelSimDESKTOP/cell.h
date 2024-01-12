#pragma once
#include "pch.h"

#if true
// https://cdn.discordapp.com/attachments/479398848129663005/1189276048974688266/IMG_1829.jpg?ex=659d92a1&is=658b1da1&hm=a45dd7c17859f34d19c9ca718693f681424732acbef6bff46417ebf58575244e
// do that.. 

class MaterialID { // Scope blocked Enum for safety.
public:
	enum : u8 {
		EMPTY,
		SAND,
		WATER,
		CONCRETE,
		GOL_ALIVE,
		COUNT,
	};

	static const std::vector<std::string> names = {
		"EMPTY",
		"SAND",
		"WATER",
		"CONCRETE",
		"Game of Life: Alive",
	};
};

struct Material
{
	u8 r, g, b, a;
	u16 d;
	std::vector<std::vector<u8>> variants;

	Material(u8 RED, u8 GREEN, u8 BLUE, u8 ALPHA, u16 DENSITY)
	{
		r = RED;
		g = GREEN;
		b = BLUE;
		a = ALPHA;
		d = DENSITY;
		variants = { {RED,GREEN,BLUE,ALPHA} };
	}
	Material() = default;
};

struct Cell
{	// 32 bits of data, fast!!
	u8 matID;
	bool updated;	// uses 1 byte??? should just be a bit 
	u8 variant;		// index to array of randomly generated RGBA values from material's RGBA.
	u8 data;		// extra data if needed, e.g fire temp

	Cell(u8 MATERIAL, u8 UPDATED, u8 COLOUR_VARIANT, u8 EXTRA_DATA)
	{
		matID = MATERIAL;
		updated = UPDATED;
		variant = COLOUR_VARIANT;
		data = EXTRA_DATA;
	}
	Cell() = default;
};