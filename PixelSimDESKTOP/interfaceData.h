#pragma once

#include "pch.h"

// could come up with a better system than manually adding new constexpr's each time, 
// change system of "creating textures", textures created at compile time in ID && name only??
// at runtime they're initialised into OpenGL via "createTexture"
// 
// constant expression --> at compilation changes name to value. no memory cost!!
// constexpr enum ?? isn't that what an enum is by definition.
constexpr int GAME_TEXTURE_IDX		 = 0;
constexpr int GAME_TEXTURE_ID        = 2;
constexpr int BACKGROUND_TEXTURE_IDX = 1;
constexpr int BACKGROUND_TEXTURE_ID  = 3;
constexpr int PRESENT_TEXTURE_IDX	 = 2;
constexpr int PRESENT_TEXTURE_ID	 = 4;

struct TextureData
{
	GLuint id  = 0;
	int width  = 0; 
	int height = 0;
	std::vector<GLubyte> data;

	TextureData(GLuint ID, int WIDTH, int HEIGHT, std::vector<GLubyte> DATA)
	{
		id     = ID;
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
