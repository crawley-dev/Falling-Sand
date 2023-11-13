#pragma once

#define GAME_TEXTURE_ID        2
#define BACKGROUND_TEXTURE_ID  3
#define GAME_TEXTURE_IDX	   0
#define BACKGROUND_TEXTURE_IDX 1

#include <vector>

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
	//GLuint texID = 0;
	//GLuint backgroundID = 0;
	//int texH,		  texReloadCount,  mouseX,  mouseY,  frame;
	//int texW = texH = texReloadCount = mouseX = mouseY = frame = 0;
	std::vector<TextureData> textures;

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

	bool runSim			= false;
	bool resetSim		= false;
	bool reloadGame		= false;
	bool scanTopDown	= true;
	bool playGameOfLife = false;

};
