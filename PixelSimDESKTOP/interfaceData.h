#pragma once

struct interfaceData
{
	GLuint texID;
	int texW, texH, texReloadCount, clDrawType, clDrawSize, clDrawChance, clColourVariance, clDrawShape, 
		clScaleFactor, mousePosX, mousePosY;
	bool runSim, hasSizeChanged, resetSim, doTopBot;

#if false // don't even need this lul
	interfaceData(GLuint TEXTURE_ID, int TEXTURE_WIDTH, int TEXTURE_HEIGHT, int TEXTURE_RELOADED_COUNT, 
				  int CELL_DRAW_TYPE, int CELL_DRAW_SIZE, int CELL_DRAW_CHANCE, int CELL_COLOUR_VARIANCE,
				  int CELL_DRAW_SHAPE, int MOUSE_POS_X, int MOUSE_POS_Y,
				  bool RUN_SIM, bool RESET_SIM, bool HAS_SIZE_CHANGED)
	{
		texID = TEXTURE_ID;
		texW = TEXTURE_WIDTH;
		texH = TEXTURE_HEIGHT;
		texReloadCount = TEXTURE_RELOADED_COUNT;
		clDrawType = CELL_DRAW_TYPE;
		clDrawSize = CELL_DRAW_SIZE;
		clDrawChance = CELL_DRAW_CHANCE;
		clColourVariance = CELL_COLOUR_VARIANCE;
		clDrawShape = CELL_DRAW_SHAPE;
		mousePosX = MOUSE_POS_X;
		mousePosY = MOUSE_POS_Y;
		
		runSim = RUN_SIM;
		hasSizeChanged = HAS_SIZE_CHANGED;
	}
	interfaceData() = default;
#endif
};