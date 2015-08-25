#include "GameWorld.h"

using namespace Ogre;

GameWorld::GameWorld(void)
{
}

GameWorld::GameWorld(Ogre::SceneManager *mSceneMgr)
{
	Entity *mEntity;
	Camera *mCamera = mSceneMgr->getCamera("Camera");
	SceneNode *mTurretNode;

	mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	/*
	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane("ground",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);
	mEntity = mSceneMgr->createEntity("GroundEntity", "ground");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mEntity);
	mEntity->setMaterialName("Examples/Rockwall");
	mEntity->setCastShadows(false);
	*/

	mTurretNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	mCamera->setPosition(0, 500, 0);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);
}

GameWorld::~GameWorld(void)
{
}