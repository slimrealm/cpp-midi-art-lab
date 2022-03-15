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
* the Turtle class provides turtle graphics type drawing functions
* moveTo: pick up pen and move to new location
* move: with pen down, draw line of length distance, from current location in direction of current
* turtle angle
* turn: turn turtle the given angle
*/
class Turtle {
public:
	float angle, x, y, penWidth;
	float PI = 3.1415926535897932384f;

	void moveTo(float newX, float newY);
	void move(float distance, Sprite &line, std::vector<Sprite> &shapesVector);
	void turn(float turnAngle);

};

/**
* The Segments class implements a drawing algorithm that draws zigzag line segments.  The opaqueness, angle, and position on screen
* are guided by input from the MIDI controller.  Input is added to a queue as it is received and then
* popped as fast as possible through the course of the algorithm.
*/
class Segments {
public:
	float alpha = 255.f;
	float width = float(VideoMode::getDesktopMode().width);
	float height = float(VideoMode::getDesktopMode().height);
	float xPosOnScreen, yPosOnScreen;
	int updates;
	std::vector<Sprite> shapesVector;
	Sprite line; Texture lineTex;
	Color color;
	Clock updateClock, oneSecondTimer;

	static Image refreshImage;
	static Sprite refreshSprite;
	static Texture imageTex;

	Turtle turtle;
	int shapesVectorStartingIndex = 0;
	
	int durationIn, noteIn, velocityIn;

	void run(RenderWindow &window);
	void pollInput();
	void update(RenderWindow &window);
	bool grabAndPopNextInput();
	void addCorrespondingShapes(RenderWindow &window);
	void drawChunk(float xPosOnScreen, float yPosOnScreen, float startAngle,
					int howManySegments, float startLength, float turnAngleIncrement, RenderWindow &window);
	void render(RenderWindow &window);
};
