#pragma once

struct CellType
{
	/*
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
	*/
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
	//int id, x, y, v;
	unsigned int x, y;
	CellType type;
	bool flag;

	/*Cell(int ID, int X, int Y, int VELOCITY, CellType CELL_TYPE, bool HAS_MOVED_FLAG)
	{
		flag = HAS_MOVED_FLAG;
		//id = ID;
		x = X;
		y = Y;
		v = VELOCITY;
		type = CELL_TYPE;
	}*/

	Cell(int X, int Y, CellType CELL_TYPE, bool FLAG)
	{
		x = X;
		y = Y;
		type = CELL_TYPE;
		flag = FLAG;
	}
	Cell() = default;
};