#pragma once

#include "Drawables.h"
#include "Enemy.h"
#include "Hacking.h"

#define move_speed 0.003
float EnemySpeed = 0.006;

Orb orbs[4];

bool Initialize(int argc, char* argv[]);
bool InitWindow(int argc, char* argv[]);
void Resize(int w, int h);
bool InitScene();
void Render();
void GetInput();
void frameCallback(int x);
void GameLogic();
void MousePosition();
void DrawAllSans(bool behind);
void Camera();
void DrawRoom();
void DrawGameUi();
void drawLoseMessage();
void drawMainMenu();
void initLevel();
void drawWinMessage();