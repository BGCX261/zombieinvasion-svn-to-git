#include "Input.h"

using namespace Ogre;

Input::Input(void)
{
}

Input::~Input(void)
{
}

    MouseQueryListener::MouseQueryListener(RenderWindow* win, Camera* cam, SceneManager *sceneManager, CEGUI::Renderer *renderer)
        : ExampleFrameListener(win, cam, false, true), mGUIRenderer(renderer)
    {
        // Setup default variables
        mCount = 0;
        mCurrentObject = NULL;
        mLMouseDown = false;
        mRMouseDown = false;
        mSceneMgr = sceneManager;

        // Reduce move speed
        mMoveSpeed = 50;
        mRotateSpeed /= 500;

        // Register this so that we get mouse events.
        mMouse->setEventCallback(this);

        // Create RaySceneQuery
        mRaySceneQuery = mSceneMgr->createRayQuery(Ray());

        // Set result text, and default state
        mRobotMode = true;
        mDebugText = "Robot Mode Enabled - Press Space to Toggle";
    } // MouseQueryListener

    MouseQueryListener::~MouseQueryListener()
    {
        mSceneMgr->destroyQuery(mRaySceneQuery);
    }

    bool MouseQueryListener::frameStarted(const FrameEvent &evt)
    {
        // Process the base frame listener code.  Since we are going to be
        // manipulating the translate vector, we need this to happen first.
        if (!ExampleFrameListener::frameStarted(evt))
            return false;

        // Swap modes
        if(mKeyboard->isKeyDown(OIS::KC_SPACE) && mTimeUntilNextToggle <= 0)
        {
            mRobotMode = !mRobotMode;
            mTimeUntilNextToggle = 1;
            mDebugText = (mRobotMode ? String("Robot") : String("Ninja")) + " Mode Enabled - Press Space to Toggle";
        }

        // Setup the scene query
        Vector3 camPos = mCamera->getPosition();
        Ray cameraRay(Vector3(camPos.x, 5000.0f, camPos.z), Vector3::NEGATIVE_UNIT_Y);
        mRaySceneQuery->setRay(cameraRay);

        // Perform the scene query
        mRaySceneQuery->setSortByDistance(false);
        RaySceneQueryResult &result = mRaySceneQuery->execute();
        RaySceneQueryResult::iterator itr;

        // Get the results, set the camera height
        for (itr = result.begin(); itr != result.end(); itr++)
        {
            if (itr->worldFragment)
            {
                Real terrainHeight = itr->worldFragment->singleIntersection.y;
                if ((terrainHeight + 10.0f) > camPos.y)
                    mCamera->setPosition(camPos.x, terrainHeight + 10.0f, camPos.z);
                break;
            } // if
        } // for

        return true;
    }

    /* MouseListener callbacks. */
    bool MouseQueryListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
    {
        // Left mouse button up
        if (id == OIS::MB_Left)
        {
            onLeftReleased(arg);
            mLMouseDown = false;
        } // if

        // Right mouse button up
        else if (id == OIS::MB_Right)
        {
            onRightReleased(arg);
            mRMouseDown = false;
        } // else if

        return true;
    }

    bool MouseQueryListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
    {
        // Left mouse button down
        if (id == OIS::MB_Left)
        {
            onLeftPressed(arg);
            mLMouseDown = true;
        } // if

        // Right mouse button down
        else if (id == OIS::MB_Right)
        {
            onRightPressed(arg);
            mRMouseDown = true;
        } // else if

        return true;
    }

    bool MouseQueryListener::mouseMoved(const OIS::MouseEvent &arg)
    {
        // Update CEGUI with the mouse motion
        CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);

        // If we are dragging the left mouse button.
        if (mLMouseDown)
        {
            CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
            Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width),mousePos.d_y/float(arg.state.height));
            mRaySceneQuery->setRay(mouseRay);
            mRaySceneQuery->setSortByDistance(false);

            RaySceneQueryResult &result = mRaySceneQuery->execute();
            RaySceneQueryResult::iterator itr;

            for (itr = result.begin(); itr != result.end(); itr++)
                if (itr->worldFragment)
                {
                    mCurrentObject->setPosition(itr->worldFragment->singleIntersection);
                    break;
                } // if 
        } // if

        // If we are dragging the right mouse button.
        else if (mRMouseDown)
        {
            mCamera->yaw(Degree(-arg.state.X.rel * mRotateSpeed));
            mCamera->pitch(Degree(-arg.state.Y.rel * mRotateSpeed));
        } // else if

        return true;
    }

    // Specific handlers
    void MouseQueryListener::onLeftPressed(const OIS::MouseEvent &arg)
    {
        // Turn off bounding box.
        if (mCurrentObject)
            mCurrentObject->showBoundingBox(false);

        // Setup the ray scene query
        CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
        Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
        mRaySceneQuery->setRay(mouseRay);
        mRaySceneQuery->setSortByDistance(true);
        mRaySceneQuery->setQueryMask(mRobotMode ? ROBOT_MASK : NINJA_MASK);

        // Execute query
        RaySceneQueryResult &result = mRaySceneQuery->execute();
        RaySceneQueryResult::iterator itr;

        // Get results, create a node/entity on the position
        for (itr = result.begin(); itr != result.end(); itr++)
        {
            if (itr->movable && itr->movable->getName().substr(0, 5) != "tile[")
            {
                mCurrentObject = itr->movable->getParentSceneNode();
                break;
            } // if

            else if (itr->worldFragment)
            {
                Entity *ent;
                char name[16];

                if (mRobotMode)
                {
                    sprintf(name, "Robot%d", mCount++);
                    ent = mSceneMgr->createEntity(name, "robot.mesh");
                    ent->setQueryFlags(ROBOT_MASK);
                } // if
                else
                {
                    sprintf(name, "Ninja%d", mCount++);
                    ent = mSceneMgr->createEntity(name, "ninja.mesh");
                    ent->setQueryFlags(NINJA_MASK);
                } // else

                mCurrentObject = mSceneMgr->getRootSceneNode()->createChildSceneNode(String(name) + "Node", itr->worldFragment->singleIntersection);
                mCurrentObject->attachObject(ent);
                break;
            } // else if
        } // for

        // Show the bounding box to highlight the selected object
        if (mCurrentObject)
            mCurrentObject->showBoundingBox(true);
    }

	void MouseQueryListener::onLeftReleased(const OIS::MouseEvent &arg)
    {
    }

    void MouseQueryListener::onRightPressed(const OIS::MouseEvent &arg)
    {
        CEGUI::MouseCursor::getSingleton().hide();
    }

    void MouseQueryListener::onRightReleased(const OIS::MouseEvent &arg)
    {
        CEGUI::MouseCursor::getSingleton().show();
    }

CameraFrameListener::CameraFrameListener( RenderWindow* win, Camera* cam, SceneManager *sceneMgr )
        : ExampleFrameListener(win, cam, false, false)
    {
        // key and mouse state tracking
        mMouseDown = false;
        mToggle = 0.0;

        // Populate the camera and scene manager containers
        mCamera = cam;
        mSceneMgr = sceneMgr;

        // set the rotation and move speed
        mRotate = 0.13;
        mMove = 250;
    }

    // Overriding the default processUnbufferedKeyInput so the key updates we define
    // later on work as intended.
	bool CameraFrameListener::processUnbufferedKeyInput(const FrameEvent& evt)
    {
        return true;
    }

    // Overriding the default processUnbufferedMouseInput so the Mouse updates we define
    // later on work as intended. 
	bool CameraFrameListener::processUnbufferedMouseInput(const FrameEvent& evt)
    {
        return true;
    }

	bool CameraFrameListener::frameStarted(const FrameEvent &evt)
    {
        mKeyboard->capture();

        mToggle -= evt.timeSinceLastFrame;

		Vector3 transVector = Vector3::ZERO;
        Vector3 transVectorVert = Vector3::ZERO;
		Vector3 transVectorHorz = Vector3::ZERO;

		// Camera moves up and down on the global Z axis
        if (mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W))
		{
            transVectorVert.z -= mMove;
			mCamera->move(transVectorVert * evt.timeSinceLastFrame);
		}
		if (mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S))
		{
            transVectorVert.z += mMove;
			mCamera->move(transVectorVert * evt.timeSinceLastFrame);
		}

		// Camera moves left and right on the global X axis
        if (mKeyboard->isKeyDown(OIS::KC_LEFT) || mKeyboard->isKeyDown(OIS::KC_A))
		{
            transVectorHorz.x -= mMove;
			mCamera->move(transVectorHorz * evt.timeSinceLastFrame);
		}
        if (mKeyboard->isKeyDown(OIS::KC_RIGHT) || mKeyboard->isKeyDown(OIS::KC_D))
		{
            transVectorHorz.x += mMove;
			mCamera->move(transVectorHorz * evt.timeSinceLastFrame);
		}

        return true;
    }