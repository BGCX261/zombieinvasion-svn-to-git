#pragma once
#include "ogreuserdefinedobject.h"

class GameObject :
	public Ogre::UserDefinedObject
{
public:
	GameObject(void);
	~GameObject(void);
};
