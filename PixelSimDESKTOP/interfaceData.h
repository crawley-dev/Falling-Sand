#pragma once

struct interfaceData
{
	GLuint textureID;
	int textureWidth, textureHeight, textureReloadCount, drawType, drawSize, drawChance, drawColourVariance, drawShape, 
		scaleFactor, mouseX, mouseY;
	bool runSim, resetSim, reloadGame, scanTopDown;

#if false // don't even need this lul
	interfaceData(GLuint TEXTURE_ID, int TEXTURE_WIDTH, int TEXTURE_HEIGHT, int TEXTURE_RELOADED_COUNT, 
				  int CELL_DRAW_TYPE, int CELL_DRAW_SIZE, int CELL_DRAW_CHANCE, int CELL_COLOUR_VARIANCE,
				  int CELL_DRAW_SHAPE, int MOUSE_POS_X, int MOUSE_POS_Y,
				  bool RUN_SIM, bool RESET_SIM, bool HAS_SIZE_CHANGED)
	{
		texID = TEXTURE_ID;
		textureWidth = TEXTURE_WIDTH;
		textureHeight = TEXTURE_HEIGHT;
		textureReloadCount = TEXTURE_RELOADED_COUNT;
		drawType = CELL_DRAW_TYPE;
		drawSize = CELL_DRAW_SIZE;
		drawChance = CELL_DRAW_CHANCE;
		drawColourVariance = CELL_COLOUR_VARIANCE;
		drawShape = CELL_DRAW_SHAPE;
		mouseX = MOUSE_POS_X;
		mouseY = MOUSE_POS_Y;
		
		runSim = RUN_SIM;
		hasSizeChanged = HAS_SIZE_CHANGED;
	}
	interfaceData() = default;
#endif
};