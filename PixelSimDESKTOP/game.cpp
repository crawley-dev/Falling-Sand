#pragma once

#include "game.h"

Game::Game() {}
Game::~Game() {}

void Game::init(std::vector<GLubyte> pixelData, int textureW, int textureH)
{
	textureData = pixelData;
	texW = textureW;
	texH = textureH;

//	Types[0] = AIR	 = PixelType(0, 255, 255, 255, 255, 0	);
	Types[0] = AIR	 = PixelType(0, 188, 231, 255, 255, 0	);
	Types[1] = SAND	 = PixelType(1, 245, 215, 176, 255, 1600);
	Types[2] = WATER = PixelType(2, 20 , 20 , 255, 100, 997	);
	Types[3] = ROCK  = PixelType(3, 200, 200, 200, 255, 2000);

	// init all pixels as air.
	for (int y = 0; y < texH; ++y)
		for (int x = 0; x < texW; ++x)
			createPixel(25, false, texIdx(x, y), x, y, 0);

	for (Pixel& p : pixels) {
		updatePixel(p.x, p.y, p.type.r, p.type.g, p.type.b, p.type.a);
		p.flag = false;
	}
}

void Game::update()
{
	calcNewPositions();
	for (Pixel& p : pixels)
	{
		if (p.type.id == 1 || 2) return;
		updatePixel(p.x, p.y, p.type.r, p.type.g, p.type.b, p.type.a);
		p.flag = false;
	}
}

void Game::calcNewPositions()
{
	//
}

void Game::createPixel(int range, bool flag, int texIdx, int x, int y, int PixelTypeID)
{
	pixels.push_back(Pixel(flag, texIdx, x, y, varyPixelColour(range, PixelTypeID)));
}

void Game::updatePixel(int x, int y, int r, int g, int b, int a)
{
	int idx = texIdx(x, y);
	if (texIdx(texW - 1, texH - 1) > idx) 
	{
		textureData[idx + 0] = r;
		textureData[idx + 1] = g;
		textureData[idx + 2] = b;
		textureData[idx + 3] = a;
	}
}

PixelType Game::varyPixelColour(int range, int PixelTypeID)
{
	PixelType material = Types[PixelTypeID];
	material.r = material.r - rand() % range;
	material.g = material.g - rand() % range;
	material.b = material.b - rand() % range;
	material.a = material.a - rand() % range;
	return material;
}

void Game::swapPixels(int x1, int y1, int x2, int y2)
{
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
	pixels[pixIdx(x, y)].type = varyPixelColour(20, PixelTypeID);
	pixels[pixIdx(x, y)].flag = true;
}


void Game::mouseDraw(int x, int y, int radius, int PixelTypeID)
{
	//int chance = 10
	int r2 = radius * radius;
	int area = r2 << 2;
	int rr = radius << 1;

	for (int i = 0; i < area; i++)
	{
		int tx = (i % rr) - radius;
		int ty = (i / rr) - radius;
		changePixelType(x + tx, y + ty, PixelTypeID);
	}
}