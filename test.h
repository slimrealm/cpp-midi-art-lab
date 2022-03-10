#pragma once

#include <iostream>
#include <queue>
#include <SFML/Graphics.hpp>
#include <SDKDDKVer.h>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace sf;

class Test {
public:
	int fps = 100;
	int circleCount = 0, defaultCircleCount = 0;
	int startingIndex = 0;
	int numUpdates = 60 * fps; 	//run for a full minute
	float startPosition = 0.f;
	float positionMarker = startPosition;
	float finalPosition = (float)VideoMode::getDesktopMode().width;
	float interval = (finalPosition - startPosition) / numUpdates;
	Texture circleTex; Sprite circles[1000];
	Texture defaultCircleTex; Sprite defaultCircles[10000];
	Texture markerTex; Sprite progressionMarker;
	int updates = 0;
	int frames = 0;

	void run(RenderWindow &window, int &width, int &height, int testChoice);
	void update(RenderWindow &window);
	void render(RenderWindow &window);
	void pollInput(bool &done);

};
