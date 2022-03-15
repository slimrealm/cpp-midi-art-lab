#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include "spiral.h"
#include "segments.h"
#include "forest.h"

using namespace sf;

/**
* MIDI_art_lab class -- get window dimensions, define drawing algorithms, declare main functions
*/
class MIDI_art_lab {

public:
	int width = VideoMode::getDesktopMode().width;
	int height = VideoMode::getDesktopMode().height;
	const int NONE = 0;
	const int RUN_AGAIN = 1, SWITCH_ALGORITHM = 2, EXIT = 99;
	int algorithm = NONE;

	Spiral spiral;
	Segments segments;
	Forest forest;

	void run(RenderWindow &window);
	void homeScreen();
	int testMenu(RenderWindow &window);
	int chooseAlgorithm(RenderWindow &window);
};
