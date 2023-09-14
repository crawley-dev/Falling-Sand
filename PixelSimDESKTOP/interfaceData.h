#pragma once

struct interfaceData
{
	//textureID, textureWidth, textureHeight, texReloadedCount, runSim, pxDrawType, pxDrawSize
	GLuint texID;
	int texW, texH, texReloadCount, clDrawType, clDrawSize;
	bool runSim, hasSizeChanged;

#if true
	interfaceData(GLuint TEXTURE_ID, int TEXTURE_WIDTH, int TEXTURE_HEIGHT, int TEXTURE_RELOADED_COUNT, 
						   int CELL_DRAW_TYPE, int CELL_DRAW_SIZE, bool RUN_SIM, bool HAS_SIZE_CHANGED)
	{
		texID = TEXTURE_ID;
		texW = TEXTURE_WIDTH;
		texH = TEXTURE_HEIGHT;
		texReloadCount = TEXTURE_RELOADED_COUNT;
		clDrawType = CELL_DRAW_TYPE;
		clDrawSize = CELL_DRAW_SIZE;
		
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