#pragma once
#include <Ogre.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <OgreCEGUIRenderer.h>
#include "ExampleApplication.h"

class Input
{
public:
	Input(void);
	~Input(void);
};

class MouseQueryListener : public ExampleFrameListener, public OIS::MouseListener
{

public:

enum QueryFlags
    {
        NINJA_MASK = 1<<0,
        ROBOT_MASK = 1<<1
    };

    MouseQueryListener(RenderWindow* win, Camera* cam, SceneManager *sceneManager, CEGUI::Renderer *renderer);

    ~MouseQueryListener();

    bool frameStarted(const FrameEvent &evt);

    /* MouseListener callbacks. */
    bool mouseMoved(const OIS::MouseEvent &arg);

    bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	/* Specific handlers */
    void onLeftPressed(const OIS::MouseEvent &arg);

	void onLeftReleased(const OIS::MouseEvent &arg);

	void onRightPressed(const OIS::MouseEvent &arg);

	void onRightReleased(const OIS::MouseEvent &arg);


protected:
    RaySceneQuery *mRaySceneQuery;     // The ray scene query pointer
    bool mLMouseDown, mRMouseDown;     // True if the mouse buttons are down
    int mCount;                        // The number of robots on the screen
    SceneManager *mSceneMgr;           // A pointer to the scene manager
    SceneNode *mCurrentObject;         // The newly created object
    CEGUI::Renderer *mGUIRenderer;     // CEGUI renderer
    bool mRobotMode;                   // The current state
};

class CameraFrameListener : public ExampleFrameListener
{
public:
    CameraFrameListener( RenderWindow* win, Camera* cam, SceneManager *sceneMgr );

    bool processUnbufferedKeyInput(const FrameEvent& evt);

    // Overriding the default processUnbufferedMouseInput so the Mouse updates we define
    // later on work as intended. 
    bool processUnbufferedMouseInput(const FrameEvent& evt);

    bool frameStarted(const FrameEvent &evt);

protected:
    bool mMouseDown;       // Whether or not the left mouse button was down last frame
    Real mToggle;          // The time left until next toggle
    Real mRotate;          // The rotate constant
    Real mMove;            // The movement constant
    SceneManager *mSceneMgr;   // The current SceneManager
    Camera *mCamera;   // The SceneNode the camera is currently attached to
	Ogre::Radian mYaw;
	Ogre::Radian mPitch;
};