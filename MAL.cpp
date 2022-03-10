#include "MAL.h"

int testChoice = 0;
int screen;
const int HOME = 1, TEST_MENU = 2, CHOOSE_ALGORITHM = 3, LCT = 4, SPIRAL = 5, FOREST = 6, SEGMENTS = 7, LIMBO = 15;
bool isTest = false;

/**
* run() function
* 
* calls homeScreen() function, then allows user to choose drawing algorithm, then runs corresponding algorithm
* @param RenderWindow &window
*/
void MIDI_art_lab::run(RenderWindow &window) {

	bool running = false;
	running = true;
	screen = HOME;

	while (running) {
		homeScreen();
		while (screen >= HOME) {
			while (algorithm == NONE && isTest) {
				testChoice = 0;
				testChoice = testMenu(window);
				if (testChoice > 1) {
					//test.run(window, width, height, testChoice);
				}
			}
			testChoice = 0;
			algorithm = chooseAlgorithm(window);
				switch (algorithm) {

				case LCT:
					screen = LCT;
					//linearCircleTest.run(window, width, height);
					break;

				case SPIRAL:
					std::cout << "\nSTARTING SPIRAL\n\n";
					screen = SPIRAL;
					spiral.run(window, width, height);
					window.clear();
					window.display();
					break;

				case FOREST:
					std::cout << "\nSTARTING FOREST\n\n";
					screen = FOREST;
					forest.run(window, width, height);
					window.clear();
					window.display();
					break;

				case SEGMENTS:
					std::cout << "\nSTARTING SEGMENTS\n\n";
					screen = SEGMENTS;
					segments.run(window);
					window.clear();
					window.display();
					break;

				default:
					break;
				}
		} //while screen >= HOME
	} //while running

}

/**
* homeScreen() function
*
* loads home screen (unfinished)
*/
void MIDI_art_lab::homeScreen() {
	screen = HOME;
	static bool connectionValid = false;
}

/**
* testMenu() function
*
* user chooses one of four project tests or the choose algorithm screen
@ return int choice
*/
int MIDI_art_lab::testMenu(RenderWindow &window) {
	screen = TEST_MENU;
	int choice = NONE;;
	/*
	1. Choose Algorithm
	2. MIDI Controller Bounds Test
	3. MIDI Byte Integrity
	4. Framerate Test
	5. Correctly Place in Algorithm
	*/

	Event event;
	Texture menuTex;
	menuTex.loadFromFile("Images/menu.png");
	Sprite menuSprite(menuTex);
	menuSprite.setPosition(350.f, 150.f);
	window.clear();
	window.draw(menuSprite);
	window.display();

	while (choice == NONE) {
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Num1)) {
					choice = 1;
					algorithm = LIMBO;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Num2)) {
					choice = 2;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Num3)) {
					choice = 3;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Num4)) {
					choice = 4;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Num5)) {
					choice = 5;
				}
			}
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}
	
	if (choice == 2 || choice == 3) {
		Texture consoleTex;
		consoleTex.loadFromFile("Images/console.png");
		Sprite consoleSprite(consoleTex);
		consoleSprite.setPosition(400.f, 150.f);
		window.clear();
		window.draw(consoleSprite);
		window.display();
	}
	else if (choice == 4 || choice == 5) {
		window.clear();
		window.display();
	}
	return choice;
}

/**
* chooseAlgorithm() function
*
* returns user's choice of algorithm as an int
* user clicks on algorithm of choice
@ return int choice
*/
int MIDI_art_lab::chooseAlgorithm(RenderWindow &window) {
	screen = CHOOSE_ALGORITHM;
	int choice = NONE;;

	Event event;
	Texture algoChooseTex;
	algoChooseTex.loadFromFile("Images/algoChoose.png");
	Sprite algoChooseSprite(algoChooseTex);
	algoChooseSprite.setPosition(350.f, 150.f);
	window.clear();
	window.draw(algoChooseSprite);
	window.display();

	while (choice == NONE) {
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Num1)) {
					choice = SPIRAL;
					algorithm = SPIRAL;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Num2)) {
					choice = SEGMENTS;
					algorithm = SEGMENTS;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Num3)) {
					choice = FOREST;
					algorithm = FOREST;
				}
			}
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}
	
	screen = choice;
	Texture spaceTex;
	spaceTex.loadFromFile("Images/space.png");
	Sprite spaceSprite(spaceTex);
	spaceSprite.setPosition(350.f, 150.f);
	window.clear();
	window.draw(spaceSprite);
	window.display();
	bool ready = false;
	while (!ready) {
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Space)) {
					ready = true;
				}
			}
		}
	}

	return choice;
}
