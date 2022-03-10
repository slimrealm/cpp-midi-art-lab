#include "segments.h"

const int HOME = 1, TEST_MENU = 2, CHOOSE_ALGORITHM = 3, LCT = 4, SPIRAL = 5, FOREST = 6, SEGMENTS = 7, LIMBO = 15;
std::queue<int> inputQueue;
Clock inputClock;
static DWORD note, velocity, duration, noteOnOrOff;
int notesPressed[110];
static bool firstNotePlayedYet = false;
bool resetRefreshSprite = false;

extern int screen;
extern int testChoice, screen;

/**
* CALLBACK MidiInProc() function
*
* Upon receiving a MIDI byte(s), this function is called
* it grabs each MIDI parameter (pitch, velocity, duration) and assigns them to
* appropriate draing parameters.  It pushes these numbers into a queue to be popped
* by the drawing algorithm
* @param HMIDIIN hMidiIn
* @param UINT wMsg
* @param DWORD dwInstance
* @param DWORD dwParam1
* @param DWORD dwParam2
*/
void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	static bool init = false;
	
	static int velocities[110];
	if (init == false) {
		for (int i = 0; i < 109; i++) {
			notesPressed[i] = 0;
			velocities[i] = 0;
		}
		init = true;
	}

	static int ON = 1, OFF = 0, OTHER = 3;
	static int onOff = OTHER;
	const int HOME = 1, TEST_MENU = 2, CHOOSE_ALGORITHM = 3, LCT = 4, SPIRAL = 5, STARBURST = 6, SEGMENTS = 7, LIMBO = 15;
	switch (wMsg) {
	case MIM_OPEN:
	//	printf("wMsg=MIM_OPEN\n");
		break;
	case MIM_CLOSE:
	//	printf("wMsg=MIM_CLOSE\n");
		break;
	case MIM_DATA:
		onOff = OTHER;

		duration = 40;

		noteOnOrOff = dwParam1 & 0x000000FF;
		note = (dwParam1 >> 8) & 0x000000FF;
		velocity = (dwParam1 >> 16) & 0x000000FF;
		if (noteOnOrOff == 144) {
			onOff = ON;
			//record timestamp for that note
			notesPressed[note] = inputClock.getElapsedTime().asMilliseconds();
			velocities[note] = velocity;
		}
		else if (noteOnOrOff == 128) {
			onOff = OFF;
			duration = (inputClock.getElapsedTime().asMilliseconds() - notesPressed[note]) / 20;
			notesPressed[note] = 0;
		}
		if (screen == TEST_MENU) {
			if (noteOnOrOff == 144 && testChoice == 2)
				std::cout << "note: " << note << "\n";

			else if (testChoice == 3 && (noteOnOrOff == 128 || noteOnOrOff == 144)) {
				std::cout << "note: " << note << ", \t";
				if (noteOnOrOff == 144)
					std::cout << "velocity: " << velocity << ",";
				else std::cout << "\t";
				std::cout << "\t\tnoteOnOrOff: " << noteOnOrOff << "\n";
			}
		}

		if (onOff == ON){
			if (screen == FOREST) {
				inputQueue.push(int(duration));
				inputQueue.push(int(note));
				inputQueue.push(int(velocity));
				inputQueue.push(onOff);
			}
		}
		else if (onOff == OFF){
		// grab duration stop time for this note
			std::cout << "duration: " << duration << "\n";
			velocity = velocities[note];
			inputQueue.push(int(duration));
			inputQueue.push(int(note));
			inputQueue.push(int(velocity));
			if (screen == FOREST) {
				inputQueue.push(onOff);
			}
		}

		if (!firstNotePlayedYet)
			firstNotePlayedYet = true;
		break;
	case MIM_LONGDATA:
	//	printf("wMsg=MIM_LONGDATA\n");
		break;
	case MIM_ERROR:
	//	printf("wMsg=MIM_ERROR\n");
		break;
	case MIM_LONGERROR:
	//	printf("wMsg=MIM_LONGERROR\n");
		break;
	case MIM_MOREDATA:
	//	printf("wMsg=MIM_MOREDATA\n");
		break;
	default:
		//printf("wMsg = unknown\n");
		break;
	}
	return;
}


//FUNCTIONS OF SEGMENTS CLASS

/**
* run() function
*
* algorithm moves from left side of screen to right side.  Line shape is instantiated, and main
* drawing loops, each iteration collecting MIDI input and then displaying the next frame
* to the screen.
* @param RenderWindow &window
*/
void Segments::run(RenderWindow &window) {

	xPosOnScreen = 0;
	resetRefreshSprite = true;
	firstNotePlayedYet = false;

	std::cout << "HERE\n";

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

	updateClock.restart();
	oneSecondTimer.restart();
	srand((unsigned)time(NULL));
	color.r = 255; color.g = 255; color.b = 0; color.a = 255;
	line.setColor(color);
	lineTex.loadFromFile("Images/Mallow.png");//("leaf2.png");
	line.setTexture(lineTex);
	line.setOrigin(2.f, 0.f);

	//MAIN LOOP
	bool drawingActive = true;
	static int ticksPerSecond = 40;
	updates = 0;
	while (drawingActive && xPosOnScreen <= width) {
			
		//every second, show framerate and reset counter and clock
		if (oneSecondTimer.getElapsedTime().asSeconds() >= 1.f) {
			oneSecondTimer.restart();
		}
		pollInput();
		//draw each subsequent frame at proper fps
		if (updates < (ticksPerSecond * updateClock.getElapsedTime().asSeconds())) {
			update(window);
			xPosOnScreen++;
		}

		render(window);
	}
	
	Texture saveTex;
	if (!saveTex.create( (unsigned int)width, (unsigned int)height)) {
		std::cout << "Image problem\n";
		while (1) {}
		exit(1);
	}

	//update texture from screen and save it to file
	Image saveImage;
	saveImage.create((unsigned int)width, (unsigned int)height, Color::Black);
	window.display();
	saveTex.update(window);
	saveImage = saveTex.copyToImage();
	if (!saveImage.saveToFile("Images/savedLeaf.png")) {
		std::cout << "Image problem\n";
		while (1) {}
		exit(1);
	}

	midiInStop(hMidiDevice);
	midiInClose(hMidiDevice);
	hMidiDevice = NULL;
}

/**
* pollInput() function
*
* Check the USB MIDI connection for new incoming data.  If there is new data, a CALLBACK is triggered and
* the new pitch, velocity, and status bytes are pushed to the input queue for later access by the
* grabAndPopNextInput() function.
*/
void Segments::pollInput() {
	//check for MIDI bytes
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
} //pollInput

/**
* update() function
*
* This is called on every tick of the drawing algorithm.  The input queue is checked, and if not empty,
* the next duration, pitch, and velocity values are popped, and the addCorrespondingShapes() function is
* called to add the next set of zigzag lines to the ShapesVector for drawing. 
* @param RenderWindow &window
*/
void Segments::update(RenderWindow &window) {
	if (!inputQueue.empty()) {
		std::cout << "inputQueue has data\n";
		if (grabAndPopNextInput())
			addCorrespondingShapes(window); //next chunk of segments calculated and added to shapesVector
	}
	updates++;
}

/**
* grabAndPopNextInput() function
*
* Checks input queue -- if not empty, the next durationIn, noteIn, and velocityIn values are popped. These
* will be used in the addCorrespondingShapes function.
*/
bool Segments::grabAndPopNextInput() {
	if (inputQueue.empty())
		return false;
	durationIn = inputQueue.front();
	inputQueue.pop();
	noteIn = inputQueue.front();
	inputQueue.pop();
	velocityIn = inputQueue.front();
	inputQueue.pop();
	std::cout << "durationIn" << durationIn << "noteIn" << noteIn << "velocityIn" << velocityIn << "\n";
	return true;
}

/**
* addCorrespondingShapes() function
*
* The numerical values durationIn, noteIn, and velocityIn are used to determine the opacity, angle, and starting y-position
* of the next chunk of zigzag lines.  
* @param RenderWindow &window
*/
void Segments::addCorrespondingShapes(RenderWindow &window) {
	std::cout << "Adding corresponding Shape\n";
	//update drawing variables based on input
	static bool firstNoteDone = false;

	//Y POSITION BASED ON PITCH
	static float yRange = float(height) - 100.f; // from 50 to height - 50
	static float noteRange = 87.f; //108-21
	static float yPixelsPerNote = yRange / noteRange;
	float yPosOnScreen = height - (50 + (yPixelsPerNote * (noteIn - 21)));

	//HOWMANYSEGMENTS AND STARTLENGTH BASED ON DURATION
	//duration range 1/20 to 3 seconds (original integer values 1 to 60)
	float howManySegmentsRange;
	int howManySegments = (durationIn / 3) + 1;
	howManySegmentsRange = 20.f;// (1 - 21)
	float startLengthRange = 45.f; //(5 - 50)
	float lengthPerSegment = startLengthRange / howManySegmentsRange;
	float startLength = 40 - (lengthPerSegment * howManySegments);

	//BRIGHTNESS / OPAQUENESS BASED ON VELOCITY (and start angle?)
	float startAngle;
	if (velocityIn >= 20 && velocityIn <= 107) {
		startAngle = (velocityIn - 20) * (127.f / 88.f);
		startAngle *= (360.f / 127.f);
	}
	else if (velocityIn < 30)
		startAngle = 0;
	else if (velocityIn > 97)
		startAngle = 360;
	std::cout << startAngle << "   ";
	color.a = 2 * velocityIn;

	//TURN ANGLE INCREMENT = equation from COMBINATION of howManySegments AND yPosOnScreen
	float turnAngleIncrement = 10;
	drawChunk(xPosOnScreen, yPosOnScreen, startAngle, howManySegments, startLength, turnAngleIncrement, window);
	if (!firstNoteDone)
		firstNoteDone = true;
}

/**
* drawChunk() function
*
* A set of zigzag lines are drawn, based on the parameters given.  The colors, number of lines,
* angle changes of the zigzag, and segment lengths are all based on these parameters.  Each line
* is added to the shapesVector for drawing.
* @param float xPosOnScreen
* @param float yPosOnScreen
* @param float startAngle
* @param int howManySegments
* @param float startLength
* @param float turnAngleIncrement
* @param RenderWindow &window
*/
void Segments::drawChunk(float xPosOnScreen, float yPosOnScreen, float startAngle,
	int howManySegments, float startLength, float turnAngleIncrement, RenderWindow &window) {
	
	color.g = 0;
	static int chunkIterations = 1;
	float turnAngle = startAngle;

	//move turtle TO proper x,y
	turtle.moveTo(xPosOnScreen, yPosOnScreen);

	//set turtle angle to proper starting angle
	turtle.angle = startAngle; //in terms of SFML rotation angle

	//set proper starting distance
	float length = startLength;
	for (int section = 1; section <= howManySegments; section++) {
		line.setColor(color);
		turtle.penWidth = 2.f;
		turtle.move(length, line, shapesVector);

		//updates for next segment
		length *= 1.1f;

		bool nextNeg;
		turnAngle >= 0 ? nextNeg = true : nextNeg = false;
		turnAngle = (abs(turnAngle));
		if (nextNeg)
			turnAngle *= -1.f;
		color.g += 12;
	}
	chunkIterations++;
}

/**
* render() function
*
* Draw to window all shapes that are currently in the shapesVector
* @param RenderWindow &window
*/
void Segments::render(RenderWindow &window) {
	static bool initialized = false;
	static Sprite refreshSprite;
	static Texture imageTex;
	static int width = VideoMode::getDesktopMode().width;
	static int height = VideoMode::getDesktopMode().height;
	if (!initialized) {
		if (!imageTex.create(width, height))
		{
			std::cout << "IMAGE PROBLEM.";
			while (1) {}
			exit(1);
		}
		initialized = true;
	}
	window.clear();
	if (resetRefreshSprite) {
		imageTex.update(window);
		refreshSprite.setTexture(imageTex);
		resetRefreshSprite = false;
		shapesVector.clear();
	}

	window.draw(refreshSprite);

	if (!shapesVector.empty()) {
		unsigned int shapes;
		for (shapes = 0; shapes < shapesVector.size(); shapes++) {
			window.draw(shapesVector[shapes]);
		}
		if (shapes >= 100) {
			//update texture from screen and refresh it to screen
			imageTex.update(window);
			refreshSprite.setTexture(imageTex);
			shapesVector.clear();
		}
	}
	window.display();
}


//FUNCTIONS OF TURTLE CLASS

/**
* moveTo() function
*
* moves turtle to a new location without drawing
* to the screen.
* @param newX
* @param newY
*/
void Turtle::moveTo(float newX, float newY) {
	x = newX; y = newY;
}

/**
* move() function
*
* moves turtle to new location AFTER adding a line to the shapesVector at the turtle's old
* location using the turtle's angle
* @param float distance
* @param Sprite &line
* @param std::vector<Sprite> &shapesVector
*/
void Turtle::move(float distance, Sprite &line, std::vector<Sprite> &shapesVector) {
	float thetaRad = (90.f - angle) * (PI / 180.f);
	//line.setScale(penWidth / 5.f, distance / 5.f); // 5 is width and height of .png file
	line.setScale(distance / 120.f, distance / 120.f); // 5 is width and height of .png file
	line.setPosition(x, y);
	line.setRotation(angle + 180.f);

	shapesVector.push_back(line);

	x = x + (distance * cos(thetaRad));
	y = y - (distance * sin(thetaRad));
}

/**
* turn() function
*
* turns the turtle by a specified angle
* @param float turnAngle
*/
void Turtle::turn(float turnAngle) {
	angle += turnAngle;
}