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

bool Space_input = false;

bool Shift_input = false;

bool Escape_input = false;
bool Escape_was_pressed = false;

bool paused = false;

float player_x;
float player_ele;
float player_z;

float angle_rot;
float angle_height;

float focal_x;
float focal_y;
float focal_z;

bool wall_rotated = false;
float wall_x = -1.0;
float wall_z = 0.0;
float door_distance = 0.5;
float door_width = 1.0;

Enemy enemy;

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

    enemy = Enemy();

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
    

    /*
    glColor3f(1.0, 1.0, 0.5);
    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glTranslatef(0.0f, 0.0f, 0.5f);
    drawWall();
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0.0f, 0.0f, 0.5f);
    drawWall();
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 0, 0, 0);
    glTranslatef(0.0f, 0.0f, 0.5f);
    drawWall();
    glPopMatrix();
    */


    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        glutLeaveMainLoop();  // request to leave the main loop
        return;
    }

    glFlush();
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

            float angle = atan(x_dist / z_dist);

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

                float angle = atan(x_dist / z_dist);

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

            if (angle_height > 1.7) {
                angle_height = 1.7;
            }
            else if (angle_height < -1.7)
            {
                angle_height = -1.7;
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