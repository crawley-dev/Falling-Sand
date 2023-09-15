#pragma once

struct interfaceData
{
	//textureID, textureWidth, textureHeight, texReloadedCount, runSim, pxDrawType, pxDrawSize
	GLuint texID;
	int texW, texH, texReloadCount, clDrawType, clDrawSize, clDrawChance, clColourVariance;
	bool runSim, hasSizeChanged, resetSim;

#if true
	interfaceData(GLuint TEXTURE_ID, int TEXTURE_WIDTH, int TEXTURE_HEIGHT, int TEXTURE_RELOADED_COUNT, 
				  int CELL_DRAW_TYPE, int CELL_DRAW_SIZE, int CELL_DRAW_CHANCE, int CELL_COLOUR_VARIANCE,
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
		
		runSim = RUN_SIM;
		hasSizeChanged = HAS_SIZE_CHANGED;
	}
	interfaceData() = default;
#endif

};

#if false
	GLuint textureID, int textureWidth, int textureHeight, int texReloadedCount, bool& runSim, int& pxDrawType,
	int& drawSize

	bool runSim = true;
	bool applicationRunning = false;
	bool hasSizeChanged = false;
	int texReloadedCount = 0;
	int framesSinceReload = 0;
	int pxDrawType = 1;
	int pxDrawSize = 40;
#endif