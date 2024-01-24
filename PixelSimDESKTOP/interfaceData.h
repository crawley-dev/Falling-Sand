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

class IFlags {
public:
	enum : u64 {
		RUN_SIM = 1 << 0,
		RESET_SIM = 1 << 1,
		RELOAD_GAME = 1 << 2,
		LOAD_IMAGE = 1 << 3,
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
		BOTTOM_UP_LEFT,
		BOTTOM_UP_RIGHT,
		SNAKE,
		GAME_OF_LIFE,
	};

	static const std::vector<std::string> names = {
		"Top Down",
		"Bottom Up L->R",
		"Bottom Up R->L",
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
	
	// Efficient Flag: u64 flags = 0;
	bool runSim	    = false;
	bool resetSim   = false;
	bool reloadGame = false;
	bool loadImage  = false;

	u8 scanMode			     = Scan::BOTTOM_UP_LEFT;
	u8 updateMode		     = Update::FLICKER;
	u8 drawShape		     = Shape::CIRCLE;
	u8 drawMaterial		     =  1;
	//u8 drawChance		     = 99; 
	int drawChance = 99;     
	u8 scaleFactor		     =  2;
	u8 fluidDispersionFactor =  4;
	u8 solidDispersionFactor =  2;

	u16 mouseX		         =  0;
	u16 mouseY		         =  0;
	//u16 drawSize		     = 10;
	int drawSize = 10;	     
						     
	u32 frame		         =  0;
	u32 texReloadCount       =  0;
	u32 textureChanges		 =  0;
};
