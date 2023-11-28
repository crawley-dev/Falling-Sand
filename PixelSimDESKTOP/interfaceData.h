#pragma once

// probably shouldn't use macros..
#define GAME_TEXTURE_ID        2
#define BACKGROUND_TEXTURE_ID  3
#define GAME_TEXTURE_IDX	   0
#define BACKGROUND_TEXTURE_IDX 1

//#include <vector>
//#include <string>
#include "pch.h"

struct TextureData
{
	GLuint id  = 0;
	int index  = 0; 
	int width  = 0; 
	int height = 0;
	std::vector<GLubyte> data;

	TextureData(GLuint ID, int INDEX, int WIDTH, int HEIGHT, std::vector<GLubyte> DATA)
	{
		id     = ID;
		index  = INDEX;
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

	int texReloadCount	   =  0;
	int mouseX			   =  0;
	int mouseY			   =  0;
	int frame			   =  0;
	int scaleFactor		   =  2;

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
