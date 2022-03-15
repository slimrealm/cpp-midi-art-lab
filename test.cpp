#include "test.h"

bool waitingForInput;
extern std::queue<int> inputQueue;
static bool firstNotePlayedYet = false;
extern void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
extern DWORD note, velocity, duration, noteOnOrOff;

void Test::run(RenderWindow &window, int &width, int &height, int testChoice) {
	Clock updateClock;
	Clock oneSecondTimer;
	HMIDIIN hMidiDevice = NULL;;
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

	if (testChoice == 2 || testChoice == 3) {
		bool done = false;
		Event event;
		std::cout << "\n\nPlay any note on MIDI controller (press 'q' to quit test)\n\n";
		while (!done) {
			waitingForInput = true;
			while (waitingForInput) {
				while (window.pollEvent(event))
				{
					if (event.type == sf::Event::KeyPressed) {
						if (Keyboard::isKeyPressed(Keyboard::Q)) {
							done = true;
							midiInStop(hMidiDevice);
							midiInClose(hMidiDevice);
							hMidiDevice = NULL;
							return;
						}
					}
				}
				pollInput(done);
				if (done) {
					midiInStop(hMidiDevice);
					midiInClose(hMidiDevice);
					hMidiDevice = NULL;
					return; 
				}
			}
		}
		return;
	}

	else if (testChoice == 4 || testChoice == 5) {
		window.clear();
		bool done = false;
		std::cout << "\n\nPlay any notes on MIDI controller (press 'q' to quit test)\n\n";

		//load progression marker
		markerTex.loadFromFile("Images/marker.png");
		progressionMarker.setTexture(markerTex);
		progressionMarker.setOrigin(2, 0);

		//load circles
		circleTex.loadFromFile("Images/circle.png");
		for (int i = 0; i < 1000; i++) {
			circles[i].setTexture(circleTex);
			float midX = circles[i].getGlobalBounds().width / 2;
			float midY = circles[i].getGlobalBounds().height / 2;
			circles[i].setOrigin(midX, midY);
		}

		//reset clocks
		oneSecondTimer.restart();
		updateClock.restart();

		while (!done) {
			//stop after 1 minute
			if (updateClock.getElapsedTime().asSeconds() > 60.f) {
					std::cout << "one minute reached.";

				//update texture from screen and save it to file
				Texture saveTex;
				if (!saveTex.create(width, height)) {
					std::cout << "failed to create texture for screenshot.\n";
					while (1) {}
					exit(1);
				}
				Image saveImage;
				saveImage.create(width, height, Color::Black);
				saveTex.update(window);
				saveImage = saveTex.copyToImage();
				if (!saveImage.saveToFile("Images/savedDrawing.png")) {
					std::cout << "failed to create image for screenshot.\n";
					exit(1);
				}
				std::cout << "screenshot saved!\n";
				window.clear();
				midiInStop(hMidiDevice);
				midiInClose(hMidiDevice);
				hMidiDevice = NULL;
				done = true;
				return;
			}

			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();
				if (event.type == Event::KeyPressed) {
					if (Keyboard::isKeyPressed(Keyboard::Q)) {
						done = true;
						midiInStop(hMidiDevice);
						midiInClose(hMidiDevice);
						hMidiDevice = NULL;
						return;
					}
				}
			}

			pollInput(done);

			if (done) {
				midiInStop(hMidiDevice);
				midiInClose(hMidiDevice);
				hMidiDevice = NULL;
				return;
			}

			/* for benchmarking -- every second, show framerate and reset counter and clock
				if (oneSecondTimer.getElapsedTime().asSeconds() >= 1.f) {
				std::cout << "fps: " << frames << "\n";
				oneSecondTimer.restart();
				frames = 0;
			}*/

			//draw each subsequent frame at current fps
			if (updates < (fps * updateClock.getElapsedTime().asSeconds())) {
				update(window);
			}
			render(window);
		}
		return;
	}
	else {
		std::cout << "testChoice = " << testChoice << "\n";
		while (1) {}
		exit(1);
	}

	midiInStop(hMidiDevice);
	midiInClose(hMidiDevice);
	hMidiDevice = NULL;
}

/**
* Checks the USB MIDI connection for new incoming data.  If there is new data, a callback is triggered and
* the new pitch, velocity, and status bytes are pushed to the input queue for later access by the
* grabAndPopNextInput() function.
*/
void Test::pollInput(bool &done) {
	//check for MIDI bytes
	if (_kbhit()) {
		//Sleep(100);
		int c = _getch();
		if (c == VK_ESCAPE) {
			for (int temp = 0; temp < 100; temp++)
				std::cout << "PROBLEM!!!";
		}
		if (c == 'q') {
			done = true;
			return;
		}
	}
}

void Test::update(RenderWindow &window) {
	static int durationIn, noteIn, velocityIn;

	//draw circle using input info and pop that input from the queue
	if (!inputQueue.empty()) {
		durationIn = inputQueue.front();
		inputQueue.pop();
		noteIn = inputQueue.front();
		inputQueue.pop();
		velocityIn = inputQueue.front();
		inputQueue.pop();

		//algorithm
		circles[circleCount].setPosition(positionMarker, 500);
		circles[circleCount].setScale(1.f, 1.f);
		circleCount++;
	}
	progressionMarker.setPosition(positionMarker, 0);
	positionMarker += interval;
	updates++;
}

void Test::render(RenderWindow &window) {
	static bool initialized = false;
	static Sprite refreshSprite;
	static Image refreshImage;
	static Texture imageTex;
	static int width = VideoMode::getDesktopMode().width;
	static int height = VideoMode::getDesktopMode().height;
	if (!initialized) {
		refreshImage.create(width, height, Color::Black);
		if (!imageTex.create(width, height))
		{
			std::cout << "Image problem\n";
			while (1) {}
			exit(1);
		}
		initialized = true;
		window.clear();
		window.display();
		window.clear();
		window.display();
		imageTex.update(window);
		refreshSprite.setTexture(imageTex);
	}

	window.draw(refreshSprite);
	if (circleCount >= 1)
		window.draw(circles[circleCount - 1]);
	window.draw(progressionMarker);
	window.display();

	//test image refresh -- update texture from screen and refresh it to screen
	imageTex.update(window);
	refreshSprite.setTexture(imageTex);
	frames++;
}
