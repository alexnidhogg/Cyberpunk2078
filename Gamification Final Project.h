#pragma once

#define move_speed 0.009
float EnemySpeed = 0.002;

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