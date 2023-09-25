#pragma once

struct CellType
{
	int id, r, g, b, a, d, tV;
	
	CellType(int ID, int RED, int GREEN, int BLUE, int ALPHA, int DENSITY, int TERMINAL_VELOCITY)
	{
		id = ID;
		r = RED;
		g = GREEN;
		b = BLUE;
		a = ALPHA;
		d = DENSITY;
		tV = TERMINAL_VELOCITY;
	}
	CellType() = default;
};

struct Cell
{
	bool flag;
	int id, x, y, v;
	CellType type;

	Cell(int ID, int X, int Y, int VELOCITY, CellType CELL_TYPE, bool HAS_MOVED_FLAG)
	{
		flag = HAS_MOVED_FLAG;
		id = ID;
		x = X;
		y = Y;
		v = VELOCITY;
		type = CELL_TYPE;
	}
	Cell() = default;
};