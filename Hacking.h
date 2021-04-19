#pragma once

#include <stdlib.h>
#include <gl/freeglut.h>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include "mmsystem.h"

struct drop {
	float x = 1;
	float y = 1;
	float inc = 0.01;
	float radius = 0.5;
	float scale = 0.2;
	float rotationAngle = 0;
	float rotationInc = 1;
};

void initRain();
void drawParticleShape(int i);
void drawDrop(int i);
void drawRain();
void drawBorder();
void calcFPS();
bool GetInput2();
void GameLogic2();
void menuScreen();
void frameCallback2(int i);
bool HackingLogic();
void initInput();
void ResetWords();
