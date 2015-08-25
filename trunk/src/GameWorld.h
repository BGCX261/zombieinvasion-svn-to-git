#pragma once
#include <Ogre.h>
#include "GameObject.h"

class GameWorld
{
public:
	GameWorld(void);
	GameWorld(Ogre::SceneManager *mSceneMgr);
	~GameWorld(void);

	Ogre::Vector3 getPointOnPlane();

private:
	//GameObject gridContents[1000][1000];
	Ogre::Plane plane;
};