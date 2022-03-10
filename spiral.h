#pragma once

#include <queue>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SDKDDKVer.h>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace sf;

/**
* Spiral class
*
* get window dimensions, define drawing algorithms, declare main functions
*/
class Spiral {
public:
	Texture tempTex;
	Sprite tempShape;
	Color currColor = Color::Black;
	int durationIn, noteIn, velocityIn;
	Texture arrowTex;
	Sprite arrow;
	float burstX[32], burstY[32];
	Texture burstTex;
	Sprite burst[32];
	int totalMarkerTicks = 0;
	int TICKS_PER_ARROW = 10;
	bool drawingActive;
	Clock drawingClock;
	std::vector<Sprite> shapesVector;
	Texture startingTexture;
	Sprite startingSprite;

	float PI = 3.1415926535897932384f;
	int totalIterations = 0;
	int shapesVectorStartingIndex = 0;
	float MARKER_TICKS_PER_SECOND = 100.f;
	float SECONDS_IN_FIRST_QUADRANT = 4.f;
	float TICKS_IN_FIRST_QUADRANT = MARKER_TICKS_PER_SECOND * SECONDS_IN_FIRST_QUADRANT;

	int TR = 1, BR = 2, BL = 3, TL = 4;
	int quadrant = TR;
	float ADJUSTMENT_WIDTH = 20.f;	//ADJUST THIS TO EXPERIMENT WITH DIFFERENT SPIRAL DENSITIES
	float leftAsymptote = (float)(VideoMode::getDesktopMode().width / 2.f);
	float rightAsymptote = leftAsymptote + ADJUSTMENT_WIDTH;
	float topAsymptote = (float)(VideoMode::getDesktopMode().height / 2.f);
	float bottomAsymptote = topAsymptote + ADJUSTMENT_WIDTH;
	int currNoOfQuadrantTicks;
	int ticksInCurrentQuadrant = (int)TICKS_IN_FIRST_QUADRANT;
	float markerX = leftAsymptote;
	float markerY = 0.f;
	float originalRadius = topAsymptote;
	float radius;
	float thetaDegrees, thetaRadians;

	void run(RenderWindow &window, int &width, int &height);
	void refreshStartingSprite(RenderWindow &window);
	void awaitFirstMIDIBytes();
	void pollInput();
	bool update(RenderWindow &window);
	bool grabAndPopNextInput();
	void addCorrespondingShapes();
	bool timeForMarkerTick();
	void MarkerTick(RenderWindow &window);
	bool timeForNextArrow();
	void addNextArrow();
	bool timeToSwitchQuadrants();
	void switchQuadrants();
	void render(RenderWindow &window);
	void saveToImage();
};