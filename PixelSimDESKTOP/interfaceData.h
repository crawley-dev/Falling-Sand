#pragma once
#include "pch.h"

// enum classes create a new type, with no implicit casting, 
//	even when explicitly stating their equivalent type???
// So I created scope blocked enums instead via nesting the enums

class TexIndex { // Scope Blocked Enum.
public:
	enum {
		GAME,
		BACKGROUND,
		PRESENTED
	};
};

class TexID { // Scope Blocked Enum.
public:
	enum {
		GAME = 2,
		BACKGROUND,
		PRESENTED
	};
};

struct TextureData
{
	GLuint id  = 0;
	int width  = 0; 
	int height = 0;
	std::vector<GLubyte> data;

	TextureData(GLuint ID, int WIDTH, int HEIGHT, std::vector<GLubyte> DATA)
	{
		id	   = ID;
		width  = WIDTH;
		height = HEIGHT;
		data   = DATA;
	}
	TextureData() = default;
};

struct interfaceData
{
	std::vector<TextureData> textures;
	std::string imagePath;

	int texReloadCount     =  0;
	int mouseX		       =  0;
	int mouseY		       =  0;
	int frame		       =  0;
	int scaleFactor	       =  2;

	int drawShape	       =  0;
	int drawType		   =  1;
	int drawSize		   = 10;
	int drawChance		   = 99;
	int drawColourVariance = 10;

	bool runSim			   = false;
	bool resetSim		   = false;
	bool reloadGame		   = false;
	bool playGameOfLife	   = false;
	bool loadImage		   = false;
	bool scanTopDown	   = true;
};
