// Gamification Final Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <math.h>
#include <Windows.h>

#include <gl/freeglut.h>

#include "Drawables.h"
#include "Gamification Final Project.h"
#include "Enemy.h"



int g_WindowId = 0;

bool W_input = false;
bool A_input = false;
bool S_input = false;
bool D_input = false;

bool W_was_pressed = false;
bool A_was_pressed = false;
bool S_was_pressed = false;
bool D_was_pressed = false;

bool Mouse_Clicked = false;
bool Mouse_was_clicked = false;

bool Space_input = false;

bool Shift_input = false;

bool Escape_input = false;
bool Escape_was_pressed = false;

int Ammo = 6;
int MaxAmmo = 6;

bool paused = false;

float player_x;
float player_ele;
float player_z;

float angle_rot;
float angle_height;

float focal_x;
float focal_y;
float focal_z;

int health = 25;
int max_health = 50;

bool wall_rotated = false;
float wall_x = -1.0;
float wall_z = 0.0;
float door_distance = 0.5;
float door_width = 1.0;

Enemy enemy = Enemy();

/*
bool wall_rotated = true;
float wall_x = 0.0;
float wall_z = -1.0;
float door_distance = 0.5;
float door_width = 1.0;
*/

bool grounded = true;
float y_vel = 0.0;

int main(int argc, char* argv[])
{
    
    if (!Initialize(argc, argv))
    {
        std::cerr << "Failed to Initialize";
        if (g_WindowId > 0)
        {
            glutDestroyWindow(g_WindowId);
            glutMainLoopEvent();
        }
        return EXIT_FAILURE;
    }
    glutMainLoop();
    return 0;
}

bool Initialize(int argc, char* argv[])
{

    player_x = 0.0;
    player_ele = 0.0;
    player_z = 0.0;

    angle_rot = 0.0;
    angle_height = 0.0;

    focal_x = 0.0;
    focal_y = 0.0;
    focal_z = 0.0;

    enemy.exists = true;
    enemy.x = 0.0;
    enemy.z = 0.0;

    if (!InitWindow(argc, argv)) {
        return false;
    }

    if (!InitScene()) {
        return false;
    }

    return true;
}

bool InitWindow(int argc, char* argv[])
{
    glutInit(&argc, argv);

    player_x = 0.0;
    player_ele = 0.0;
    player_z = -0.9;

    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    g_WindowId = glutCreateWindow("Deus Ex 0.5: The Scuffening");
    if (g_WindowId < 1)
    {
        return false;
    }

    glutDisplayFunc(Render);
    glutReshapeFunc(Resize);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    frameCallback(0);

    return true;
}

void Resize(int w, int h)
{
    std::cout << "Resizing to " << w << "x" << h << std::endl;

    // define which part of the window we're drawing to
    glViewport(0, 0, w, h);

    // define projection
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

bool InitScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    initTextures();

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return false;
    }

    return true;
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GetInput();
    std::cout << A_input << W_input << S_input << D_input << '\n';
    MousePosition();
    GameLogic();

    gluPerspective(45.0, 800.0 / 600.0, 0.005, 5.0);
    
    gluLookAt(
        player_x, player_ele, player_z,
        player_x + focal_x, player_ele + focal_y, player_z + focal_z,
        0.0f, 1.0f, 0.0f
    );

    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.0f);
    drawWall();
    glPopMatrix();
    
    //glColor3f(1.0, 0.5, 0.0);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.0f);
    drawWall();
    glPopMatrix();

    //glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f);
    glRotatef(90, 0, 1, 0);
    drawWall();
    glPopMatrix();

    //glColor3f(1.0, 1.0, 0.0);
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f);
    glRotatef(90, 0, 1, 0);
    drawWall();
    glPopMatrix();

    glColor3f(0.0, 0.0, 1.0);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.0f);
    glRotatef(90, 1, 0, 0);
    drawCieling();
    glPopMatrix();

    glColor3f(0.0, 1.0, 0.0);
    glPushMatrix();
    glTranslatef(0.0f, -0.1f, 0.0f);
    glRotatef(90, 1, 0, 0);
    drawFloor();
    glPopMatrix();

    DrawAllSans(true);
    drawInnerWall(wall_x, wall_z, door_distance, -wall_x, -wall_z, door_width, wall_rotated);
    DrawAllSans(false);
    
    //UI

    unsigned char AmmoCount[2];
    strcpy_s((char*)AmmoCount, 2, std::to_string(Ammo).c_str());

    unsigned char MaxAmmoCount[2];
    strcpy_s((char*)MaxAmmoCount, 2, std::to_string(MaxAmmo).c_str());

    unsigned char* Divider = (unsigned char*)"/";

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    float pos_x = 0.0;
    float pos_y = 0.0;
    float pos_z = 0.0;

    pos_x = player_x + sin(angle_rot) * 2;
    pos_y = player_ele + sin(angle_height) * 2;
    pos_z = player_z - cos(angle_rot) * 2;
    
    glRasterPos3f(pos_x, pos_y, pos_z);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"\n\n\n\n\n\n\n\n\n\n                                                        ");
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, AmmoCount);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, Divider);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, MaxAmmoCount);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)" ~ Ammo");

    glRasterPos3f(pos_x, pos_y, pos_z);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"\n\n\n\n\n\n\n\n\n\n\n\n                     ");
    for (int i = 0; i < round(float(max_health - health)*0.8); i++)
    {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)" ");
    }
    for (int i = 0; i < health; i++) 
    {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"|");
    }
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)" ~ Health");

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        glutLeaveMainLoop();  // request to leave the main loop
        return;
    }

    glutSwapBuffers();

}

void frameCallback(int x)
{
    glutPostRedisplay();
    glutTimerFunc(15, frameCallback, 0);
}

void GetInput()
{
    W_input = (GetKeyState(0x57) & 0x8000);
    A_input = (GetKeyState(0x41) & 0x8000);
    S_input = (GetKeyState(0x53) & 0x8000);
    D_input = (GetKeyState(0x44) & 0x8000);

    Mouse_Clicked = (GetKeyState(VK_LBUTTON) & 0x8000);

    Escape_input = (GetKeyState(VK_ESCAPE) & 0x8000);

    Space_input = (GetKeyState(VK_SPACE) & 0x8000);

    if ((GetKeyState(VK_LSHIFT) & 0x8000) || (GetKeyState(VK_RSHIFT) & 0x8000))
    {
        Shift_input = true;
    }
    else
    {
        Shift_input = false;
    }
    /*
    UP_input = (GetKeyState(VK_UP) & 0x8000);
    LEFT_input = (GetKeyState(VK_LEFT) & 0x8000);
    DOWN_input = (GetKeyState(VK_DOWN) & 0x8000);
    RIGHT_input = (GetKeyState(VK_RIGHT) & 0x8000);

    SPACE_input = (GetKeyState(VK_SPACE) & 0x8000);
    I_input = (GetKeyState(0x49) & 0x8000);
    */
}

void GameLogic()
{
    if (!paused) 
    {
        if (grounded)
        {
            if (Space_input)
            {
                grounded = false;
                y_vel = 0.006;
            }
        }
        else
        {
            player_ele += y_vel;
            y_vel -= 0.0003;
            if (player_ele <= 0.0)
            {
                player_ele = 0.0;
                grounded = true;
                y_vel = 0.0;
            }
        }

        if (W_input)
        {
            if (Shift_input)
            {
                player_z -= cos(angle_rot) * move_speed / 2;
                player_x += sin(angle_rot) * move_speed / 2;
            }
            player_z -= cos(angle_rot) * move_speed;
            player_x += sin(angle_rot) * move_speed;
        }

        if (S_input)
        {
            player_z += cos(angle_rot) * move_speed;
            player_x -= sin(angle_rot) * move_speed;
        }

        if (A_input)
        {
            player_z -= sin(angle_rot) * move_speed;
            player_x -= cos(angle_rot) * move_speed;
        }

        if (D_input)
        {
            player_z += sin(angle_rot) * move_speed;
            player_x += cos(angle_rot) * move_speed;
        }
    }
   

    if (Escape_input)
    {
        if (!Escape_was_pressed)
        {
            paused = !paused;
        }
        Escape_was_pressed = true;
    }
    else
    {
        Escape_was_pressed = false;
    }


    if (player_x > 0.95)
    {
        player_x = 0.95;
    }
    else if (player_x < -0.95)
    {
        player_x = -0.95;
    }

    if (player_z > 0.95)
    {
        player_z = 0.95;
    }
    else if (player_z < -0.95)
    {
        player_z = -0.95;
    }

    
    if (wall_rotated) 
    {
        if (abs(player_x - wall_x) < 0.05)
        {
            if (player_z < wall_z + door_distance || player_z > wall_z + door_distance + door_width)
            {
                if (player_x > wall_x)
                {
                    player_x = wall_x + 0.05;
                }
                else
                {
                    player_x = wall_x - 0.05;
                }
            }
        }
    }
    else
    {
        if (abs(player_z - wall_z) < 0.05)
        {
            if (player_x < wall_x + door_distance || player_x > wall_x + door_distance + door_width)
            {
                if (player_z > wall_z)
                {
                    player_z = wall_z + 0.05;
                }
                else
                {
                    player_z = wall_z - 0.05;
                }
            }
        }
    }

    focal_x = sin(angle_rot);
    focal_y = sin(angle_height);
    focal_z = -cos(angle_rot);

    //Animate Sans

    if (enemy.exists)
    {
        if (player_z > wall_z == enemy.z > wall_z)
        {

            //Same side
            float x_dist = enemy.x - player_x;
            float z_dist = enemy.z - player_z;

            float angle = atan2(x_dist, z_dist);

            if (player_x < enemy.x)
            {
                enemy.x -= abs(sin(angle) * EnemySpeed);
            }
            else 
            {
                enemy.x += abs(sin(angle) * EnemySpeed);
            }
                
            if (player_z < enemy.z)
            {
                enemy.z -= abs(cos(angle) * EnemySpeed);
            }
            else
            {
                enemy.z += abs(cos(angle) * EnemySpeed);
            }
                
        }
        else
        {
            //Opposite side
            if (enemy.x < door_distance + wall_x + 0.05)
            {
                //before door
                if (enemy.z > wall_z) {
                    enemy.x += EnemySpeed / 1.4;
                    enemy.z -= EnemySpeed / 1.4;
                }
                else
                {
                    enemy.x += EnemySpeed / 1.4;
                    enemy.z += EnemySpeed / 1.4;
                }
            }
            else if (enemy.x > door_distance + door_width + wall_x - 0.05)
            {
                //after door
                if (enemy.z > wall_z) {
                    enemy.x -= EnemySpeed / 1.4;
                    enemy.z -= EnemySpeed / 1.4;
                }
                else
                {
                    enemy.x -= EnemySpeed / 1.4;
                    enemy.z += EnemySpeed / 1.4;
                }
            }
            else
            {
                
                //in door
                float x_dist = enemy.x - player_x;
                float z_dist = enemy.z - player_z;

                float angle = atan2(x_dist, z_dist);

                if (player_x < enemy.x)
                {
                    enemy.x -= abs(sin(angle) * EnemySpeed);
                }
                else
                {
                    enemy.x += abs(sin(angle) * EnemySpeed);
                }

                if (player_z < enemy.z)
                {
                    enemy.z -= abs(cos(angle) * EnemySpeed);
                }
                else
                {
                    enemy.z += abs(cos(angle) * EnemySpeed);
                }

            }
        }
    }
}

void MousePosition()
{
    POINT p;

    int win_x = glutGet(GLUT_WINDOW_X);
    int win_y = glutGet(GLUT_WINDOW_Y);
    int win_w = glutGet(GLUT_WINDOW_WIDTH);
    int win_h = glutGet(GLUT_WINDOW_HEIGHT);

    if (GetCursorPos(&p))
    {
        if (!paused) {
            ShowCursor(false);
            p.x -= win_x;
            p.y -= win_y;
            p.x -= win_w / 2;
            p.y -= win_h / 2;

            angle_rot += float(p.x) / win_w * 2;
            angle_height += -float(p.y) / win_h * 2;

            if (angle_height > 1.0) {
                angle_height = 1.0;
            }
            else if (angle_height < -1.0)
            {
                angle_height = -1.0;
            }

            SetCursorPos(win_x + (win_w / 2), win_y + (win_h / 2));
        }
    }
}

void DrawAllSans(bool behind) {

    if (enemy.exists)
    {
        if ((player_z > wall_z == enemy.z > wall_z) != behind) {
            glColor3f(1.0, 0.0, 0.0);
            glPushMatrix();
            float relative_angle = atan2(player_z - enemy.z, player_x - enemy.x) / 3.14156 * 180;
            glTranslatef(enemy.x, 0.0f, enemy.z);
            glRotatef(relative_angle + 90, 0.0, -1.0, 0.0);
            DrawSans();
            glPopMatrix();
        }
    }

}