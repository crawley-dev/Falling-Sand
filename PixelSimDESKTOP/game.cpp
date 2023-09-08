#pragma once

#include "game.h"

Game::Game() {}
Game::~Game() {}

void Game::init(std::vector<GLubyte> pixelData, int textureW, int textureH)
{
	textureData = pixelData;
	texW = textureW;
	texH = textureH;

	Types[0] = AIR	 = PixelType(0, 255, 255, 255, 255, 0	);
//	Types[0] = AIR	 = PixelType(0, 188, 231, 255, 255, 0	);
	Types[1] = SAND	 = PixelType(1, 245, 215, 176, 255, 1600);
	Types[2] = WATER = PixelType(2, 20 , 20 , 255, 100, 997	);
	Types[3] = ROCK  = PixelType(3, 200, 200, 200, 255, 2000);

	const int initType = 0;
	// init all pixels as air.
	for (int y = 0; y < texH; ++y)
		for (int x = 0; x < texW; ++x) {
			createPixel(initType, false, texIdx(x, y), x, y, 0);
			updatePixel(x,y, Types[initType].r, Types[initType].g, Types[initType].b, Types[initType].a);
			// p.flag = false
		}
}

void Game::reload(std::vector<GLubyte> pixelData, int newTexW, int newTexH)
{
	const int deltaW = newTexW - texW;
	const int deltaH = newTexH - texH;
	texW = newTexW;
	texH = newTexH;
}

void Game::update()
{
	for (int y = texH - 1; y > 0; --y)
		for (int x = 0; x < texW - 1; ++x) {
			Pixel p = pixels[pixIdx(x, y)];

			calcNewPosition(p);
			updatePixel(x, y, p.type.r, p.type.g, p.type.b, p.type.a);
			pixels[pixIdx(x, y)].flag = false;
		}

	/*calcNewPositions();
	for (Pixel& p : pixels)
	{
		if (p.type.id == 0 || p.type.id == 3) continue; // AIR || ROCK
		updatePixel(p.x, p.y, p.type.r, p.type.g, p.type.b, p.type.a);
		p.flag = false; // whats the point of set true then straight to false ??
	}*/
}

void Game::calcNewPosition(Pixel p)
{
	if (p.type.id == 0 || p.type.id == 3) return; // Air | Rock
	else if (p.type.id == 1) // Sand
	{
		if		(canSwapPixels(p.x, p.y, p.x, p.y + 1));		// check vertical
		else if (canSwapPixels(p.x, p.y, p.x - 1, p.y + 1));	// check bot left
		else if (canSwapPixels(p.x, p.y, p.x + 1, p.y + 1));	// check bot right
	}
}

void Game::createPixel(int range, bool flag, int texIdx, int x, int y, int PixelTypeID)
{
	if (outOfBounds(x, y)) return;
	pixels.push_back(Pixel(flag, texIdx, x, y, varyPixelColour(range, PixelTypeID)));
}

void Game::updatePixel(int x, int y, int r, int g, int b, int a)
{
	if (outOfBounds(x, y)) return;

	int idx = texIdx(x, y);
	textureData[idx + 0] = r;
	textureData[idx + 1] = g;
	textureData[idx + 2] = b;
	textureData[idx + 3] = a;
}

PixelType Game::varyPixelColour(int range, int PixelTypeID)
{
	if (range <= 0) return Types[PixelTypeID];
	PixelType material = Types[PixelTypeID];
	material.r = material.r - rand() % range;
	material.g = material.g - rand() % range;
	material.b = material.b - rand() % range;
	material.a = material.a - rand() % range;
	return material;
}

void Game::swapPixels(int x1, int y1, int x2, int y2)
{
	if (outOfBounds(x1, y1) || outOfBounds(x2,y2)) return;

	Pixel& p1 = pixels[pixIdx(x1, y1)];
	Pixel& p2 = pixels[pixIdx(x2, y2)];

	if (!p1.flag && !p2.flag) {
		int id1 = p1.type.id;
		p1.type = p2.type;
		p2.type = Types[id1];

		p1.flag = true;
		p2.flag = true;
	}
}

void Game::changePixelType(int x, int y, int PixelTypeID)
{
	if (outOfBounds(x, y)) return;
	pixels[pixIdx(x, y)].type = varyPixelColour(20, PixelTypeID);
	pixels[pixIdx(x, y)].flag = true;
}

void Game::mouseDraw(int x, int y, int radius, int PixelTypeID)
{
	int r2 = radius * radius;
	int area = r2 << 2; // *4
	int rr = radius << 1; // *2

	for (int i = 0; i < area; i++)
	{
		int tx = (i % rr) - radius;
		int ty = (i / rr) - radius;
		changePixelType(x + tx, y + ty, PixelTypeID);
	}
}

bool Game::canSwapPixels(int x1, int y1, int x2, int y2) 
{ 
	if (pixels[pixIdx(x1, y1)].type.d > pixels[pixIdx(x2, y2)].type.d &&
		pixels[pixIdx(x1, y1)].flag == false && pixels[pixIdx(x2,y2)].flag == false) 
	{
		swapPixels(x1, y1, x2, y1);
		return true;
	}
	return false;
}
