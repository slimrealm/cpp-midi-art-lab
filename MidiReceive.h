#pragma once

#include <SDKDDKVer.h>
#include <Windows.h>

#include <stdio.h>
#include <conio.h>
#include <iostream>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class MidiReceive {
public:
	void PrintMidiDevices();

	void CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};
