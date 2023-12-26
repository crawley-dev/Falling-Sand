#pragma once

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