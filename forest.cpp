#include "forest.h"

extern bool waitingForInput;
extern int testChoice, screen;
extern void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
extern DWORD note, velocity, duration, noteOnOrOff;
extern std::queue<int> inputQueue;
extern int* notesPressed;

void Forest::run(RenderWindow &window, int &width, int &height) {
	srand((unsigned)time(NULL));
	std::cout << "RUNNING FOREST\n";

	//reset these variables every time forest runs
	Color darkGreen(51,37,2);
	x = float(width / 2);
	y = float(height / 2);
	currColor.r = 0; currColor.g = 255; currColor.b = 0;
	trajDeg = 270.f;
	stage = 1;
	trajRad = trajDeg * conv;
	shapesList.clear();
	window.clear(darkGreen);
	window.display();

	//MIDI setup
	HMIDIIN hMidiDevice = NULL;
	DWORD nMidiPort = 0;
	UINT nMidiDeviceNum;
	MMRESULT rv;
	nMidiDeviceNum = midiInGetNumDevs();
	if (nMidiDeviceNum == 0) {
		fprintf(stderr, "midiInGetNumDevs() return 0...");
		while (1) {}
		exit(1);
	}
	rv = midiInOpen(&hMidiDevice, nMidiPort, (DWORD)(void*)MidiInProc, 0, CALLBACK_FUNCTION);
	if (rv != MMSYSERR_NOERROR) {
		fprintf(stderr, "midiInOpen() failed...rv=%d", rv);
		while (1) {}
		exit(1);
	}
	midiInStart(hMidiDevice);

	treeTex.loadFromFile("Images/leaf2.png");
	tree.setTexture(treeTex);
	tree.setOrigin(float(treeTex.getSize().x / 2), float(treeTex.getSize().y / 2));
	rockTex.loadFromFile("Images/Mallow.png");
	rock.setTexture(rockTex);
	rock.setOrigin(float(rockTex.getSize().x / 2), float(rockTex.getSize().y / 2));

	drawingActive = true;
	drawingClock.restart();

	//main loop
	while (drawingActive) {
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Q)) {
					drawingActive = false;
				}
			}
		}
		pollInput();
		tick();
		update(window);
		render(window);
	}

	//save to file
	Texture saveTex;
	if (!saveTex.create((unsigned int)width, (unsigned int)height)) {
		std::cout << "Image problem\n";
		while (1) {}
		exit(1);
	}
	Image saveImage;
	saveImage.create((unsigned int)width, (unsigned int)height, Color::Black);
	window.display();
	saveTex.update(window);
	saveImage = saveTex.copyToImage();
	if (!saveImage.saveToFile("Images/savedForest.png")) {
		std::cout << "Image problem\n";
		while (1) {}
		exit(1);
	}

	midiInStop(hMidiDevice);
	midiInClose(hMidiDevice);
	hMidiDevice = NULL;
}

//listen for MIDI bytes
void Forest::pollInput() {
	if (_kbhit()) {
		int c = _getch();
		if (c == VK_ESCAPE) {
			for (int temp = 0; temp < 100; temp++)
				std::cout << "PROBLEM!!!";
		}
		if (c == 'q') {
			for (int temp = 0; temp < 100; temp++)
				std::cout << "PROBLEM!!!";
		}
	}
}

void Forest::tick() {
	if (drawingClock.getElapsedTime().asMilliseconds() > tickDeltaT) {
		//change x and y based on trajectory
		x += (cos(trajRad) * dist);
		y += (sin(trajRad) * dist);
		drawingClock.restart();
	}

}

void Forest::update(RenderWindow &window) {
	if (!inputQueue.empty()) {
		grabAndPopNextInput();
		addPressReleaseShapes();
	}
}

void Forest::grabAndPopNextInput() {
	durationIn = inputQueue.front();
	inputQueue.pop();
	noteIn = inputQueue.front();
	inputQueue.pop();
	velocityIn = inputQueue.front();
	inputQueue.pop();
	onOff = inputQueue.front();
	inputQueue.pop();
}

void Forest::addPressReleaseShapes() {
	//Pressed key
	if (onOff == 1) {
		//change trajectory based on noteIn
		angleChange = float((noteIn - 65)) / 5.f; //88-key piano note range: 21 <--> 108
		trajDeg += angleChange;
		trajRad = trajDeg * conv;

		//set shape thickness based on velocityIn
		fatness = float(velocityIn / 64.f);

		//Draw one tree at cursor
		tree.setScale(fatness, 1);
		tree.setPosition(x,y);
		if (shapesList.size() > 10000) {
			drawingActive = false;
		}
		tree.setColor(currColor);
		shapesList.push_back(tree);
	}

	//Released key
	else if (onOff == 0) {
		//Color trees (& rocks) assoc. with this note press
		if (durationIn > 30) {
			if (currColor.b <= 205 && stage == 1) {
				currColor.b += 50;
			}
			else if (currColor.r <= 205 && stage < 3) {
				stage = 2;
				currColor.r += 50;
				currColor.b -= 50;
			}
			else if (currColor.g >= 50){
				stage = 3;
				currColor.g -= 50;
			}
			else {
				stage = 4;
				if (currColor.r >= 50 && currColor.g <= 205) {
					currColor.r -= 50;
					currColor.g += 50;
				}
				else {
					currColor.r = 0;
					currColor.g = 255;
					currColor.b = 0;
					stage = 1;
				}
			}
		}
	}
}

void Forest::render(RenderWindow &window) {
	Color darkGreen(51, 37, 2);
	window.clear(darkGreen);
	if (!shapesList.empty()) {
		for (std::list<Sprite>::iterator it = shapesList.begin(); it != shapesList.end(); ++it) {
			window.draw(*it);
		}
	}
	window.display();
}
