#pragma once
#include "gameobject.h"

class Zombie :
	public GameObject
{
public:
	Zombie(void);
	~Zombie(void);
private:
	int health;
};
