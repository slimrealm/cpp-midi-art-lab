#include "spiral.h"

bool spiralInit;
bool firstPass;
extern bool waitingForInput;
extern int testChoice, screen;
extern void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
extern DWORD note, velocity, duration, noteOnOrOff;
extern std::queue<int> inputQueue;

void Spiral::run(RenderWindow &window, int &width, int &height) {
	srand((unsigned)time(NULL));

	firstPass = false;
	spiralInit = false;
	currColor = Color::Black;
	totalMarkerTicks = 0;
	totalIterations = 0;
	shapesVectorStartingIndex = 0;
	quadrant = TR;
	leftAsymptote = (float)(VideoMode::getDesktopMode().width / 2.f);
	rightAsymptote = leftAsymptote + ADJUSTMENT_WIDTH;
	topAsymptote = (float)(VideoMode::getDesktopMode().height / 2.f);
	bottomAsymptote = topAsymptote + ADJUSTMENT_WIDTH;
	ticksInCurrentQuadrant = (int)TICKS_IN_FIRST_QUADRANT;
	markerX = leftAsymptote;
	markerY = 0.f;
	radius = topAsymptote - markerY;

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

	arrowTex.loadFromFile("Images/Arrow.png");
	arrow.setTexture(arrowTex);
	arrow.setOrigin(16, 8);
	burstTex.loadFromFile("Images/bigLeaf.png");
	for (int i = 0; i < 32; i++) {
		burst[i].setTexture(burstTex);
		burst[i].setOrigin(151, 296);
	}
	drawingActive = true;
	drawingClock.restart();

	//main loop
	while (drawingActive) {	
		pollInput();
		if (update(window)) {
			render(window);
		}
		totalIterations++;
	}

	Texture saveTex;
	if (!saveTex.create((unsigned int)width, (unsigned int)height)) {
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
	if (!saveImage.saveToFile("Images/savedSpiral.png")) {
		std::cout << "Image problem\n";
		while (1) {}
		exit(1);
	}

	midiInStop(hMidiDevice);
	midiInClose(hMidiDevice);
	hMidiDevice = NULL;
}

void Spiral::refreshStartingSprite(RenderWindow &window) {
	startingTexture.update(window);
	startingSprite.setTexture(startingTexture);
}

void Spiral::awaitFirstMIDIBytes() {
	while (inputQueue.empty()) {
		pollInput();
	}
}

//check for MIDI bytes
void Spiral::pollInput() {
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

bool Spiral::update(RenderWindow &window) {
	bool needToRender = false;
	if (timeForMarkerTick()) { //sets next (x,y) point on the spiral
		MarkerTick(window);
	}
	if (!inputQueue.empty()) {
		grabAndPopNextInput();
		addCorrespondingShapes(); //this sets correct position, rotation, color, scale etc. of shape(s)
		needToRender = true;
	}
	if (timeForNextArrow()) { //every TICKS_PER_ARROW ticks, draw another arrow at correct rotation
		addNextArrow();
		needToRender = true;
	}
	if (timeToSwitchQuadrants()) {
		switchQuadrants();
		needToRender = true;
	}
	return needToRender;
}

bool Spiral::grabAndPopNextInput() {
	durationIn = inputQueue.front();
	inputQueue.pop();
	noteIn = inputQueue.front();
	inputQueue.pop();
	velocityIn = inputQueue.front();
	inputQueue.pop();
	return true;
}

void Spiral::addCorrespondingShapes() {
	//update drawing variables based on input
	int layers = 4 - (noteIn / 30); //note values 21-108 become layers 5-1, based on 20-note chunks
	int satelliteSpacing = (noteIn + 4) % 12;
	if (satelliteSpacing == 0)
		satelliteSpacing = 12;
	float scalingFactor = (float)(velocityIn) / 100.f;
	float rotation = 90.f - thetaDegrees;

	int burstIndex = 1;  //0
	int currLayer = 1;
	burstX[burstIndex] = markerX;
	burstY[burstIndex] = markerY;

	burst[burstIndex].setPosition(burstX[burstIndex], burstY[burstIndex]);
	burst[burstIndex].setColor(currColor);
	burst[burstIndex].setRotation(rotation);
	burst[burstIndex].setScale(scalingFactor, scalingFactor);

	if (layers > 1) {
		float baseXSpacing = (satelliteSpacing * 20) * cos(thetaRadians);
		float baseYSpacing = (satelliteSpacing * 20) * sin(thetaRadians);
		for (int currLayer = 1; currLayer < layers; currLayer++) {
			//layer indices: | 1 | 2-3 | 4-7 | 8-15 | 16-31 |
			// who spawns who: | 1 -> 2,3 | 2 -> 4,5 | 3 -> 6,7 | 4 -> 8,9 | ... | 15 -> 30-31 |    n -> 2n,2n+1

			//for each index in previous layer, get satellite x,y points for two satellites
			for (int index = int(pow(2, currLayer - 1)); index < int(pow (2, currLayer)); index++) {

				//next point 1
				burstIndex++;
				burstX[burstIndex] = burstX[index] + baseXSpacing * pow(0.7f, float(currLayer));//(distance equation);	//alt: burstX[2*index]
				burstY[burstIndex] = burstY[index] - baseYSpacing * pow(0.7f, float(currLayer));//(distance equation);	//alt: burstY[2*index]
				//create & initialize satellite bursts
				burst[burstIndex].setColor(currColor);
				burst[burstIndex].setRotation(rotation);
				burst[burstIndex].setPosition(burstX[burstIndex], burstY[burstIndex]);
				burst[burstIndex].setScale(scalingFactor * pow(0.5f, float(currLayer)), scalingFactor * pow(0.5f, float(currLayer)));
				burstIndex++;

				//next point 2
				burstX[burstIndex] = burstX[index] - baseXSpacing * pow(0.7f, float(currLayer));// (distance equation);	//alt: burstX[(2*index) + 1]
				burstY[burstIndex] = burstY[index] + baseYSpacing * pow(0.7f, float(currLayer));// (distance equation);	//alt: burstY[(2*index) + 1]
				//create & initialize satellite bursts
				burst[burstIndex].setColor(currColor);
				burst[burstIndex].setRotation(rotation);
				burst[burstIndex].setPosition(burstX[burstIndex], burstY[burstIndex]);
				burst[burstIndex].setScale(scalingFactor * pow(0.5f, float(currLayer)), scalingFactor * pow(0.5f, float(currLayer)));
			}
		}
	}

	for (int i = 1; i <= burstIndex; i++) {
		shapesVector.push_back(burst[i]);
	}
}

bool Spiral::timeForMarkerTick() {
	if (drawingClock.getElapsedTime().asSeconds() * MARKER_TICKS_PER_SECOND > totalMarkerTicks)
		return true;
	else
		return false;
}

void Spiral::MarkerTick(RenderWindow &window) {
	float MARKER_TICKS = 2334.f;
	float colorsPerChunk = MARKER_TICKS / 3.5f;
	float colorShiftPerTick = 255.f / colorsPerChunk;
	static int ticksInChunk;
	static int transitionBlock;
	static float r, g, b;

	if (!firstPass) {
		ticksInChunk = totalMarkerTicks;
		transitionBlock = 1;
		r = 0; g = 0; b = 0;
		firstPass = true;
	}

	if (ticksInChunk > colorsPerChunk) {
		ticksInChunk = 0;
		transitionBlock++;
	}

	switch (transitionBlock) {
	case 1:
		r += colorShiftPerTick;
		currColor.r = Uint8(r);
		std::cout << float(currColor.r) << " ";
		std::cout << r << "\n";
		break;
	case 2:
		g += colorShiftPerTick;
		currColor.g = Uint8(g);
		break;
	case 3:
		r -= colorShiftPerTick;
		currColor.r = Uint8(r);
		break;
	case 4:
		b += colorShiftPerTick;
		currColor.b = Uint8(b);
		break;
	case 5:
		g -= colorShiftPerTick;
		currColor.g = Uint8(g);
		break;
	case 6:
		r += colorShiftPerTick;
		currColor.r = Uint8(r);
		break;
	case 7:
		g += colorShiftPerTick;
		currColor.g = Uint8(g);
		break;
	default:
		break;

	}
	ticksInChunk++;

	static float tickAngle, verticalAsymptote, horizontalAsymptote;

	const int TR = 1, BR = 2, BL = 3, TL = 4;
	switch (quadrant) {
	case TR:
		if (currNoOfQuadrantTicks == 0) {
			radius = topAsymptote - markerY;
			verticalAsymptote = leftAsymptote;
			horizontalAsymptote = topAsymptote;
			ticksInCurrentQuadrant = (int)(TICKS_IN_FIRST_QUADRANT * (radius / originalRadius));
			tickAngle = 90.f / ticksInCurrentQuadrant;
		}
		thetaDegrees = 90.f - (tickAngle * currNoOfQuadrantTicks);
		break;

	case BR:
		if (currNoOfQuadrantTicks == 0) {
			radius = markerX - rightAsymptote;
			verticalAsymptote = rightAsymptote;
			horizontalAsymptote = topAsymptote;
			ticksInCurrentQuadrant = (int)(TICKS_IN_FIRST_QUADRANT * (radius / originalRadius));
			tickAngle = 90.f / ticksInCurrentQuadrant;
		}
		thetaDegrees = 360.f - (tickAngle * currNoOfQuadrantTicks);
		break;

	case BL:
		if (currNoOfQuadrantTicks == 0) {
			radius = markerY - bottomAsymptote;
			verticalAsymptote = rightAsymptote;
			horizontalAsymptote = bottomAsymptote;
			ticksInCurrentQuadrant = (int)(TICKS_IN_FIRST_QUADRANT * (radius / originalRadius));
			tickAngle = 90.f / ticksInCurrentQuadrant;
		}
		thetaDegrees = 270.f - (tickAngle * currNoOfQuadrantTicks);
		break;

	case TL:
		if (currNoOfQuadrantTicks == 0) {
			radius = leftAsymptote - markerX;
			verticalAsymptote = leftAsymptote;
			horizontalAsymptote = bottomAsymptote;
			ticksInCurrentQuadrant = (int)(TICKS_IN_FIRST_QUADRANT * (radius / originalRadius));
			tickAngle = 90.f / ticksInCurrentQuadrant;
		}
		thetaDegrees = 180.f - (tickAngle * currNoOfQuadrantTicks);
		break;

	default:
		//exit(1);
		break;
	}

	thetaRadians = thetaDegrees * (PI / 180);
	markerX = verticalAsymptote + (radius * cos(thetaRadians));
	markerY = horizontalAsymptote - (radius * sin(thetaRadians));
	currNoOfQuadrantTicks++;
	totalMarkerTicks++;
	tempShape.setPosition(markerX, markerY);
	std::cout << "radius: " << radius << "\n";
	if (radius < 10) drawingActive = false;
}

bool Spiral::timeForNextArrow() {
	if (totalMarkerTicks % TICKS_PER_ARROW == 0)
		return true;
	else
		return false;
}

void Spiral::addNextArrow() {

	arrow.setPosition(markerX, markerY);
	//seven color chunks spaced evenly along full spiral (Black->Red | Red->Yellow | ... ... ... | Violet->White)
	arrow.setColor(currColor);	//current color will be updated IN MarkerTick() function
	float rotation = 90.f - thetaDegrees;
	arrow.setRotation(rotation);
	shapesVector.push_back(arrow);
}

bool Spiral::timeToSwitchQuadrants() {
	//reset currNoOfQuadrantTicks each time a quadrant is completed
	if (currNoOfQuadrantTicks >= ticksInCurrentQuadrant)
		return true;
	else
		return false;
}

void  Spiral::switchQuadrants() {
	quadrant++;
	//order of quadrants (spiral) = TR, BR, BL, TL, TR, BR, BL, TL, etc.
	if (quadrant > TL)
		quadrant = TR;
	currNoOfQuadrantTicks = 0;
}

void  Spiral::render(RenderWindow &window) {
	window.clear();
	if (!spiralInit) {
		shapesVector.clear();
		spiralInit = true;
	}

	if (!shapesVector.empty()) {
		for (unsigned int i = shapesVectorStartingIndex; i < shapesVector.size() - 1; i++) {
			window.draw(shapesVector[i]);
		}
	}
	window.display();
}

void  Spiral::saveToImage() {
	//implementation in Test class
}
