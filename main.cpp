#include <SFML/Graphics.hpp>
#include <iostream>
#include <windows.h>
#include "MAL.h"

using namespace sf;

/** 
* main function -- Instantiate diaplay window; instantiate and run MIDI Art Lab
*/
int main()
{
	RenderWindow window(VideoMode(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height), "MIDI Art Lab");
	window.clear();
	MIDI_art_lab * MAL = new MIDI_art_lab();
	MAL->run(window);	
	return 0;
}