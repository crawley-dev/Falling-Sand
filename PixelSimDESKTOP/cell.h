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
		COUNT
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
		variants = {};
	}
	Material() = default;
};

#if true
struct Cell
{	// 32 bits of data
	u8 matID;
	bool updated;	// uses 1 byte??? should just be a bit 
	u8 variant;	// index to array of randomly generated RGBA values from material's RGBA.
	u8 data;	// extra data if needed, e.g fire temp

	Cell(u8 MATERIAL, u8 UPDATED, u8 COLOUR_VARIANT, u8 EXTRA_DATA)
	{
		matID = MATERIAL;
		updated = UPDATED;
		variant = COLOUR_VARIANT;
		data = EXTRA_DATA;
	}
	Cell() = default;
};
#elif false
struct Cell
{
	uint8_t r, g, b, a, flags;
	Material mat;
	
	Cell(uint8_t R, uint8_t G, uint8_t B, uint8_t A, uint8_t FLAGS, Material MATERIAL)
	{
		r = R;
		g = G;
		b = B;
		a = A;
		flags = FLAGS;
		Material mat = MATERIAL;
	}
};
#endif

#elif true
struct CellType
{
	unsigned char id, r, g, b, a;
	unsigned int d;
	
	CellType(char ID, char RED, char GREEN, char BLUE, char ALPHA, int DENSITY) 
	{
		id = ID;
		r = RED;
		g = GREEN;
		b = BLUE;
		a = ALPHA;
		d = DENSITY;
	}
	CellType() = default;
};	

struct Cell
{
	unsigned int x, y;
	CellType type;
	bool flag;

	Cell(int X, int Y, CellType CELL_TYPE, bool FLAG)
	{
		x = X;
		y = Y;
		type = CELL_TYPE;
		flag = FLAG;
	}
	Cell() = default;
};
#endif