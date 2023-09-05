#pragma once

struct PixelType
{
	int id, r, g, b, a, d;
	
	PixelType(int ID, int RED, int GREEN, int BLUE, int ALPHA, int DENSITY)
	{
		id = ID;
		r = RED;
		g = GREEN;
		b = BLUE;
		a = ALPHA;
		d = DENSITY;
	}
	PixelType() = default;
};

struct Pixel
{
	bool flag;
	int id, x, y;
	PixelType type;

	Pixel(bool HAS_MOVED_FLAG, int ID, int X, int Y, PixelType PIXEL_TYPE)
	{
		flag = HAS_MOVED_FLAG;
		id = ID;
		x = X;
		y = Y;
		type = PIXEL_TYPE;
	}
};