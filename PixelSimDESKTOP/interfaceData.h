#pragma once

struct interfaceData
{
	GLuint texID = 0;
	int texH,		  texReloadCount,  mousePosX,  mousePosY,  frame;
	int texW = texH = texReloadCount = mousePosX = mousePosY = frame = 0;
	
	int clDrawShape	     =  0;
	int clDrawType		 =  1;
	int clDrawSize		 = 10;
	int clDrawChance     = 99;
	int clColourVariance = 10;
	int clScaleFactor	 =	2;

	bool runSim   = false;
	bool resetSim = false;
	bool doReload = false;
	bool doTopBot = true;

};