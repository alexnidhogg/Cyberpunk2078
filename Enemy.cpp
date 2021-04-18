
#include "Enemy.h"

#define Max_Health 3

Enemy::Enemy() 
{
	Enemy::health = Max_Health;
	Enemy::exists = false;
	Enemy::x = 0.0;
	Enemy::z = 0.0;
};
Enemy::~Enemy()
{

};