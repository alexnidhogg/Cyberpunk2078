// Gamification Final Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <math.h>
#include <Windows.h>
#include "mmsystem.h"

#include <gl/freeglut.h>

#include "Gamification Final Project.h"


bool enable_Sans = true;

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
bool Space_was_pressed = false;

bool Shift_input = false;

bool Escape_input = false;
bool Escape_was_pressed = false;

int Ammo = 9;
int MaxAmmo = 9;

int Reloading = 0;

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

int damage = 10;

bool wall_rotated = false;
float wall_x = -1.0;
float wall_z = 0.0;
float door_distance = 0.5;
float door_width = 1.0;

float pi = 3.14159265359;

int respawn = 0;

int current_orb;

//achievements
bool miss_type = false;
bool miss_shot = false;
bool get_hit = false;



enum {
    MainMenu,
    Playing,
    Typing,
    Win,
    Lose
};
int gamestate = MainMenu;


Enemy enemy = Enemy();

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

    PlaySound(TEXT("Sound/Megalovania.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
    //waveOutSetVolume(0, 0x0F00);//VOLUME INCREASE 0x0000 - 0xFFFF

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

    enemy.exists = enable_Sans;
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

    angle_rot = pi;

    focal_x = sin(angle_rot);
    focal_y = sin(angle_height);
    focal_z = -cos(angle_rot);

    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    g_WindowId = glutCreateWindow("Deus Ex 0.5: Sans' Revenge");
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

void initLevel()
{
    for (int i = 0; i < 4; i++)
    {
        orbs[i].done = false;
        orbs[i].rotation = 0.0;
        orbs[i].x = -0.5 + i % 2;
        orbs[i].y = -0.5;
        if (i > 1)
        {
            orbs[i].y += 1.0;
        }
    }
    enemy.exists = false;
    enemy.health = 6;
    respawn = 100;

    health = 25;
    Ammo = MaxAmmo;
    
    player_x = 0.0;
    player_ele = 0.0;
    player_z = 0.0;
    
    grounded = true;
    y_vel = 0.0;

    angle_rot = pi;
    angle_height = 0.0;

    miss_type = false;
    miss_shot = false;
    get_hit = false;
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
    initRain();

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
    GameLogic();

    glColor3f(0.0, 0.0, 0.0);

    switch (gamestate) {
    case MainMenu:

        drawRain();
        drawBorder();
        drawMainMenu();

        break;

    case Playing:

        Camera();
        DrawRoom();
        DrawGameUi();
        MousePosition();

        break;

    case Typing:

        drawRain();
        drawBorder();

        if (GetInput2())
        {
            miss_type = true;
        }
        GameLogic2();

        if (HackingLogic())
        {
            orbs[current_orb].done = true;
            for (int i = 0; i < 4; i++)
            {
                if (!orbs[i].done)
                {
                    gamestate = Playing;
                }
            }
            if (gamestate != Playing)
            {
                PlaySound(TEXT("Sound/Megalovania.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                gamestate = Win;
            }
            ResetWords();
        }

        break;

    case Win:

        drawRain();
        drawBorder();
        drawWinMessage();

        break;

    case Lose:

        drawRain();
        drawBorder();
        drawLoseMessage();
        
        break;

    }

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        glutLeaveMainLoop();  // request to leave the main loop
        return;
    }

    glutSwapBuffers();

}

void Camera()
{
    gluPerspective(45.0, 800.0 / 600.0, 0.005, 5.0);
    gluLookAt(
        player_x, player_ele, player_z,
        player_x + focal_x, player_ele + focal_y, player_z + focal_z,
        0.0f, 1.0f, 0.0f
    );
}

void DrawRoom()
{
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

    float sans_distance = sqrt(pow(enemy.x - player_x, 2) + pow(enemy.z - player_z, 2));
    float orbs_distance[4];

    //see how far orbs are from player
    for (int i = 0; i < 4; i++)
    {
        orbs_distance[i] = sqrt(pow(orbs[i].x - player_x, 2) + pow(orbs[i].y - player_z, 2));
    }

    //opposite side of wall

    //draw far orbs
    for (int i = 0; i < 4; i++)
    {
        if (orbs[i].y < wall_z != player_z < wall_z)
        {
            if (orbs_distance[i] > sans_distance)
            {
                if (!orbs[i].done)
                {
                    drawOrbs(&orbs[i]);
                }
                
            }
        }
    }

    //draw sans
    DrawAllSans(true);

    //draw close orbs
    for (int i = 0; i < 4; i++)
    {
        if (orbs[i].y < wall_z != player_z < wall_z)
        {
            if (orbs_distance[i] < sans_distance)
            {
                if (!orbs[i].done)
                {
                    drawOrbs(&orbs[i]);
                }
            }
        }
    }

    drawInnerWall(wall_x, wall_z, door_distance, -wall_x, -wall_z, door_width, wall_rotated);
    
    //near side of wall

    //draw far orbs
    for (int i = 0; i < 4; i++)
    {
        if (orbs[i].y < wall_z == player_z < wall_z)
        {
            if (orbs_distance[i] > sans_distance)
            {
                if (!orbs[i].done)
                {
                    drawOrbs(&orbs[i]);
                }
            }
        }
    }

    //draw sans
    DrawAllSans(false);

    //draw close orbs
    for (int i = 0; i < 4; i++)
    {
        if (orbs[i].y < wall_z == player_z < wall_z)
        {
            if (orbs_distance[i] < sans_distance)
            {
                if (!orbs[i].done)
                {
                    drawOrbs(&orbs[i]);
                }
            }
        }
    }

}

void DrawGameUi()
{
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

    glColor3f(0.0, 0.0, 0.0);
    glRasterPos3f(pos_x, pos_y, pos_z);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"o");

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glRasterPos3f(pos_x, pos_y, pos_z);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"\n\n\n\n\n\n\n\n\n\n                                                        ");
    if (Reloading <= 0) {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, AmmoCount);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, Divider);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, MaxAmmoCount);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)" ~ Ammo");
    }
    else
    {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Reloading");
        for (int i = Reloading; i < 120; i += 40)
        {
            glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)".");
        }
    }

    glRasterPos3f(pos_x, pos_y, pos_z);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"\n\n\n\n\n\n\n\n\n\n\n\n                     ");
    for (int i = 0; i < round(float(max_health - health) * 0.8); i++)
    {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)" ");
    }
    for (int i = 0; i < health; i++)
    {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"|");
    }
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)" ~ Health");

    if (paused)
    {
        glRasterPos3f(pos_x, pos_y, pos_z);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Paused");
    }
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
}

void GameLogic()
{
    switch (gamestate) {
    case MainMenu:

        if (Space_input)
        {
            if (!Space_was_pressed)
            {
                Space_was_pressed = true;
                initLevel();
                gamestate = Playing;
            }
        }
        else
        {
            Space_was_pressed = false;
        }

        break;
    case Playing:

        //Fix rotation underflow / overflow
        if (angle_rot < 0)
        {
            angle_rot += 2 * pi;
        }
        if (angle_rot > 2 * pi)
        {
            angle_rot -= 2 * pi;
        }

        //Game inputs and interacting with Sans
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

            if (Mouse_Clicked)
            {
                if (!Mouse_was_clicked)
                {
                    Mouse_was_clicked = true;
                    if (Ammo > 0)
                    {

                        PlaySound(TEXT("Sound/gun-gunshot-01.wav"), NULL, SND_ASYNC | SND_FILENAME);
                        //waveOutSetVolume(0, 0x0F00);//VOLUME INCREASE 0x0000 - 0xFFFF
                        //Shoot
                        Ammo -= 1;

                        float Angle_to_Sans = atan2(player_x - enemy.x, player_z - enemy.z);
                        float temp_Player_Rot = angle_rot;

                        float distance_to_sans = sqrt(pow(player_x - enemy.x, 2) + pow(player_z - enemy.z, 2));
                        float total_angle = Angle_to_Sans + temp_Player_Rot;

                        if (total_angle > pi) { total_angle -= 2 * pi; }

                        if (abs(total_angle) < atan2(0.1, distance_to_sans))
                        {
                            //std::cout << "Hit\n";
                            enemy.health -= 1;
                            if (enemy.health <= 0)
                            {
                                enemy.exists = false;
                                respawn = 100;
                            }
                        }
                        else
                        {
                            //std::cout << "Miss\n";
                            miss_shot = true;
                        }

                        //std::cout << angle_rot << ", " << Angle_to_Sans << " : " << total_angle << " Tan " << atan2(0.05, distance_to_sans) << "\n";

                        if (Ammo == 0)
                        {
                            Reloading = 120;
                        }
                    }
                    else
                    {
                        PlaySound(TEXT("Sound/gun-trigger-click-01.wav"), NULL, SND_ASYNC | SND_FILENAME);
                    }
                }
            }
            else
            {
                Mouse_was_clicked = false;
            }

            //Reload gun
            if (Reloading > 0)
            {
                Reloading -= 1;
                if (Reloading == 0)
                {
                    Ammo = MaxAmmo;
                }
            }

            if (respawn > 0)
            {
                respawn--;
                if (respawn == 0)
                {
                    enemy.exists = true;
                    if (player_x < 0)
                    {
                        enemy.x = 0.95;
                    }
                    else
                    {
                        enemy.x = -0.95;
                    }
                    if (player_z < 0)
                    {
                        enemy.z = 0.95;
                    }
                    else
                    {
                        enemy.z = -0.95;
                    }
                    enemy.health = 9;
                }
            }

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

                if (sqrt(pow(enemy.z - player_z, 2) + pow(enemy.x - player_x, 2)) < 0.1)
                {
                    enemy.exists = false;
                    respawn = 100;
                    health -= damage;
                    if (health <= 0)
                    {
                        gamestate = Lose;
                        PlaySound(TEXT("Sound/Megalovania.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                    }
                    else
                    {
                        std::cout << "OOF" << "\n";
                        PlaySound(TEXT("Sound/Roblox-death-sound.wav"), NULL, SND_ASYNC | SND_FILENAME);
                        get_hit = true;
                    }
                }
            }

            //run into PC
            for (int i = 0; i < 4; i++)
            {
                if (!orbs[i].done)
                {
                    if (sqrt(pow(orbs[i].x - player_x, 2) + pow(orbs[i].y - player_z, 2)) < 0.05)
                    {
                        gamestate = Typing;
                        initInput();
                        current_orb = i;
                    }
                }
            }
        }

        //Pausing / Unpausing
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

        //Collision with outside walls
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

        //Collision with inside walls
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

        //get camera point
        focal_x = sin(angle_rot);
        focal_y = sin(angle_height);
        focal_z = -cos(angle_rot);

        break;
    case Typing:
        
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

            if (sqrt(pow(enemy.z - player_z, 2) + pow(enemy.x - player_x, 2)) < 0.1)
            {
                enemy.exists = false;
                respawn = 100;
                health -= damage;
                if (health <= 0)
                {
                    gamestate = Lose;
                    PlaySound(TEXT("Sound/Megalovania.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
                }
                else
                {
                    std::cout << "OOF" << "\n";
                    PlaySound(TEXT("Sound/Roblox-death-sound.wav"), NULL, SND_ASYNC | SND_FILENAME);
                    ResetWords();
                    gamestate = Playing;
                    player_x = 0.0;
                    player_z = 0.0;

                    get_hit = true;
                }
            }
        }
        
        break;

    case Win:

        if (Space_input)
        {
            if (!Space_was_pressed)
            {
                Space_was_pressed = true;
                gamestate = MainMenu;
            }
        }
        else
        {
            Space_was_pressed = false;
        }

        break;

    case Lose:
        //Go back to main menu
        if (Space_input)
        {
            if (!Space_was_pressed)
            {
                Space_was_pressed = true;
                gamestate = MainMenu;
            }
        }
        else
        {
            Space_was_pressed = false;
        }

        break;
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

void drawLoseMessage() {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-0.1, 0.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"You lost!");
    glRasterPos2f(-0.45, -0.5);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Press Space to go back to the main menu");
    glColor3f(0.0, 0.0, 0.0);
}

void drawMainMenu()
{
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-0.3, 0.3);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Deus Ex 0.5: Sans' Revenge");
    glRasterPos2f(-0.18, -0.3);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Move with WASD");
    glRasterPos2f(-0.5, -0.4);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Aim and shoot with mouse and left mouse button");
    glRasterPos2f(-0.885, -0.5);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Touch a PC to start a typing challenge. Beat all 4 before sans drops your health to 0!");
    glRasterPos2f(-0.4, -0.6);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Press Space to Jump and start the game");
    glColor3f(0.0, 0.0, 0.0);
}

void drawWinMessage()
{
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-0.1, 0.5);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"You Won!");
    
    glRasterPos2f(-0.6, 0.2);
    if (miss_shot || miss_type || get_hit)
    {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"There are still more achievements!");
    }
    else
    {
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Congrats! All Achievements obtained!");
    }

    if (!miss_shot)
    {
        glRasterPos2f(-0.6, 0.1);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Achievement: Don't miss a single shot!");
    }

    if (!miss_type)
    {
        glRasterPos2f(-0.6, 0.0);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Achievement: Don't type anything incorrectly!");
    }

    if (!get_hit)
    {
        glRasterPos2f(-0.6, -0.1);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Achievement: Don't get hit once!");
    }

    glRasterPos2f(-0.6, -0.5);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Press Space to go back to the main menu");
    glColor3f(0.0, 0.0, 0.0);
}