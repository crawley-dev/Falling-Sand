#pragma once

#if false 
// https://cdn.discordapp.com/attachments/479398848129663005/1189276048974688266/IMG_1829.jpg?ex=659d92a1&is=658b1da1&hm=a45dd7c17859f34d19c9ca718693f681424732acbef6bff46417ebf58575244e
// do that.. 
struct MaterialDefinitives
{

};

struct Cell
{

}
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