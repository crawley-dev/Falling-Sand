#pragma once
#include "pch.h"

// enum classes create a new type, with no implicit casting, 
//	even when explicitly stating their equivalent type???
// So I created scope blocked enums instead via nesting the enums

class TexIndex { // Scope Blocked Enum
public:
	enum : u8 {
		GAME,
		BACKGROUND,
		PRESENTED,
	};
};

class TexID { // Scope Blocked Enum.
public:
	enum : u8 {
		GAME = 2,
		BACKGROUND,
		PRESENTED,
	};
};

namespace Update {
	enum : u8 {
		STATIC,
		FLICKER,
	};

	static const std::vector<std::string> names = {
		"Static",
		"Flicker",
	};
};

namespace Scan {
	enum : u8 {
		TOP_DOWN,
		BOTTOM_UP,
		SNAKE,
		GAME_OF_LIFE,
	};

	static const std::vector<std::string> names = {
		"Top Down",
		"Bottom Up",
		"Snake",
		"Game of Life",
	};
};


namespace Shape {
	enum : u8 {
		CIRCLE,
		CIRCLE_OUTLINE,
		LINE,
		SQUARE,
	};

	static const std::vector<std::string> names = {
		"Circle",
		"Circle Outline",
		"Line",
		"Square",
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
	
	bool runSim	    = false;
	bool resetSim   = false;
	bool reloadGame = false;
	bool loadImage  = false;

	u8 scanMode			   = Scan::BOTTOM_UP;
	u8 updateMode		   = Update::STATIC;
	u8 drawShape		   = Shape::CIRCLE;
	u8 drawMaterial		   =  1;
	//u8 drawChance		   = 99; 
	int drawChance = 99;
	u8 scaleFactor	       =  2;
	u8 dispersionFactor    =  8;

	u16 mouseX		       =  0;
	u16 mouseY		       =  0;
	//u16 drawSize		   = 10;
	int drawSize = 10;

	u32 frame		       =  0;
	u32 texReloadCount     =  0;
};
