#pragma once

struct CellType
{
	int id, r, g, b, a, d;
	
	CellType(int ID, int RED, int GREEN, int BLUE, int ALPHA, int DENSITY)
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
	bool flag;
	int id, x, y;
	CellType type;

	Cell(bool HAS_MOVED_FLAG, int ID, int X, int Y, CellType PIXEL_TYPE)
	{
		flag = HAS_MOVED_FLAG;
		id = ID;
		x = X;
		y = Y;
		type = PIXEL_TYPE;
	}
};