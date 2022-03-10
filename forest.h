#pragma once
#include <queue>
#include <list>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SDKDDKVer.h>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace sf;

class Forest {
public:
	int durationIn, noteIn, velocityIn, onOff;
	float x, y;

	float dist = 3.f;
	int tickDeltaT = 100;

	bool drawingActive;
	bool initialized = false;
	std::list<Sprite> shapesList;
	Clock drawingClock, refreshClock;
	Texture treeTex, rockTex, refreshTex;
	Sprite tree, rock, refreshSprite;
	float PI = 3.1415926535897932384f;
	float trajDeg, trajRad, fatness, angleChange;
	float conv = (PI/180.f);
	Event event;
	int stage;
	Color currColor;

	void run(RenderWindow &window, int &width, int &height);
	void pollInput();
	void tick();
	void update(RenderWindow &window);
	void grabAndPopNextInput();
	void addPressReleaseShapes();
	void render(RenderWindow &window);
};