#include <stdlib.h>
#include <stdio.h>
#include <gl/freeglut.h>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <time.h>
#include "mmsystem.h"

#include "Hacking.h"

bool hackStart;
std::string word2 = "TYPE THIS";
char word[] = "type this";
std::string word_array[] = { "type this", "lolcats", "sans is number one", "deus ex", "spaghetti code", "gamification", "hit boxes", "collision detection", "there is no escape", "megalovania" };

char lastKeyPressed;
std::string userInput = "temp";
std::string userText;
//bool hackStart;
double x = (double)rand() / RAND_MAX;
double y = (double)rand() / RAND_MAX;
double x_negative = ((double)rand() / RAND_MAX) * -1;
double y_negative = ((double)rand() / RAND_MAX) * -1;

bool inputArray[27];
bool backspace_input = false;
bool backspace_was_pressed = false;

int user_time = 0;

bool inputArrayWasPressed[27];
char inputArrayValues[27] = { 0x20, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A };
char wordThing[1024];
bool pick_word = false;

#define RAINSIZE 60
int counter = 0;
time_t t;
float rotationAngle = 0;

float elapsedTime = 0, base_time = 0, fps = 0, frames;

drop rain[RAINSIZE];

void initRain() {
	srand((unsigned)time(&t));
	for (int i = 0; i < RAINSIZE; i++) {
		rain[i].x = (float)rand() / RAND_MAX * (rand() % 4 + (-2));
		rain[i].y = rand() % 5;
		rain[i].inc = (float)(rand() % 25) / 1000.0;
		rain[i].radius = 10;
		rain[i].scale = (float)(rand() % 2000) / 1000.0;
		rain[i].rotationAngle = (float)(rand() % 3000) / 1000.0;
		rain[i].rotationInc = (float)(rand() % 100) / 1000.0;
		if ((rand() % 100) > 50) {
			rain[i].rotationInc = -rain[i].rotationInc;
		}
	}
}

void drawParticleShape(int i) {
	glBegin(GL_POINTS);
	glVertex2d(rain[i].x, rain[i].y);
	glEnd();
	glBegin(GL_LINES);
	glVertex2d(rain[i].x, rain[i].y);
	glVertex2d(rain[i].x, rain[i].y + rain[i].radius * 2);
	glEnd();
}

void drawDrop(int i) {
	glColor3f(0.0, 0.5, 0.0);
	glLineWidth(0.2);
	drawParticleShape(i);
	rain[i].y -= rain[i].inc;
	if (rain[i].y < -1) {
		rain[i].y = 1;
	}
}

void drawRain() {

	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	glVertex3f(-1, -1, 0);
	glVertex3f(-1, 1, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(1, -1, 0);
	glEnd();

	for (int i = 0; i < RAINSIZE; i++) {
		drawDrop(i);
	}
}

void drawBorder() {

	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0.8);
	glVertex3f(-1, -1, 0);
	glVertex3f(-1, 1, 0);
	glVertex3f(-0.9, 1, 0);
	glVertex3f(-0.9, -1, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0.8);
	glVertex3f(-1, 1, 0);
	glVertex3f(-0.9, 0.9, 0);
	glVertex3f(1, 0.9, 0);
	glVertex3f(1, 1, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0.8);
	glVertex3f(1, 1, 0);
	glVertex3f(1, -1, 0);
	glVertex3f(0.9, -1, 0);
	glVertex3f(0.9, 1, 0);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.8, 0.8, 0.8);
	glVertex3f(1, -1, 0);
	glVertex3f(0.9, -0.9, 0);
	glVertex3f(-1, -0.9, 0);
	glVertex3f(-1, -1, 0);
	glEnd();
	char terminal_name[] = "Toshiba";
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(-0.15, 0.92);
	for (int i = 0; i < sizeof(terminal_name); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, terminal_name[i]);
	}

}

void calcFPS() {
	elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	if ((elapsedTime - base_time) > 1000.0) {
		fps = frames * 1000.0 / (elapsedTime - base_time);
		//printf("fps: %f", fps);
		base_time = elapsedTime;
		frames = 0;
		user_time += 1;
		//printf("User Time Elapsed: %d", user_time);
		//std::cout << "User Time Elapsed: " << user_time << " -------------------------------" << std::endl;
		if (user_time >= 10) {
			hackStart = false;
			//std::cout << "Hacking Completed!" << std::endl;
			//InitWindow();
		}
	}
	frames++;
}

/*--------------------------------------------------------------------------------------*/
bool GetInput2() {
	for (int i = 0; i < sizeof(inputArray); i++) {
		inputArray[i] = (GetKeyState(inputArrayValues[i]) & 0x8000);
		//std::cout << i << "   " << inputArray[i] << std::endl;
	}
	backspace_input = (GetKeyState(0x08) & 0x8000);
	//A_input = (GetKeyState(0x41) & 0x8000);
	return backspace_input;
}
void GameLogic2() {
	for (int j = 0; j < sizeof(inputArray); j++) {
		if (inputArray[j]) //A Input
		{
			if (inputArrayWasPressed[j] == false) {
				inputArrayWasPressed[j] = true;
				//std::cout << inputArrayValues[j];
				//lastKeyPressed = 'a';
				userText += tolower(inputArrayValues[j]);
			}
		}
		else
		{
			inputArrayWasPressed[j] = false;
		}
	}
	if (backspace_input) //A Input
	{
		if (backspace_was_pressed == false) {
			backspace_was_pressed = true;
			//std::cout << "A";
			if (!userText.empty()) {
				userText.erase(userText.size() - 1);
			}
		}
	}
	else
	{
		backspace_was_pressed = false;
	}
}

void menuScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glRasterPos3f(x + x_negative, y + y_negative, 0);

	GameLogic2();

	calcFPS();
	//glFlush();

	drawRain();
	//counter++;

	drawBorder();
	glutPostRedisplay();

	glColor3f(0.6, 0.4, 1);
	glRasterPos2f(0.0, 0.0);

	if (!pick_word) {
		int iRand = (rand() % 9) + 1;
		strncpy_s(wordThing, word_array[iRand].c_str(), sizeof(wordThing));
		word2 = word_array[iRand];
		pick_word = true;
	}
	for (int i = 0; i < sizeof(word); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, wordThing[i]);
		//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, userWord[i]);//make user
	}

	if (userText != word2) {
		//std::cout << "User Input: " << userText << "      ||      Word: " << word2 << std::endl;
	}
	else {//Succeeded hacking, give points and leave terminal
		//std::cout << "I DID THE THING";
		//score here

		//kick back to 3d part
	}

	glutSwapBuffers();
}
void frameCallback2(int x)
{
	GetInput2();
	glutPostRedisplay();

	glutTimerFunc(33, frameCallback2, 0);
}

void initInput()
{
	for (int j = 0; j < sizeof(inputArray); j++) {
		inputArrayWasPressed[j] = true;
	}
}

void ResetWords()
{
	int iRand = (rand() % 9) + 1;
	strncpy_s(wordThing, word_array[iRand].c_str(), sizeof(wordThing));
	word2 = word_array[iRand];
	pick_word = true;
	while(!userText.empty()) {
		userText.erase(userText.size() - 1);
	}

}

bool HackingLogic()
{
	glColor3f(0.6, 0.4, 1);
	glRasterPos2f(0.0, 0.0);

	if (!pick_word) {
		int iRand = (rand() % 9) + 1;
		strncpy_s(wordThing, word_array[iRand].c_str(), sizeof(wordThing));
		word2 = word_array[iRand];
		pick_word = true;
	}
	for (int i = 0; i < sizeof(wordThing); i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, wordThing[i]);
		//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, userWord[i]);//make user
	}

	
	glColor3f(0.8, 0.2, 0.2);
	glRasterPos2f(0.0, -0.1);
	int i = 0;
	while(userText[i] != '\0'){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, userText[i]);
		i++;
	}
	


	//std::cout << userText << word2 << '\n';

	if (userText == word2)
	{
		return true;
	}
	return false;
	
}