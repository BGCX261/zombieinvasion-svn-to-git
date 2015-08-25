#include <Ogre.h>
#include <OIS/OIS.h>
#include <CEGUI/CEGUI.h>
#include <OgreCEGUIRenderer.h>
#include "GameWorld.h"
#include "Input.h"

using namespace Ogre;

class ExitListener : public FrameListener
{
public:
    ExitListener(OIS::Keyboard *keyboard)
        : mKeyboard(keyboard)
    {
    }

    bool frameStarted(const FrameEvent& evt)
    {
        mKeyboard->capture();
        return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
    }

private:
    OIS::Keyboard *mKeyboard;
};

class Application
{
public:
    void go()
    {
        createRoot();
        defineResources();
        setupRenderSystem();
        createRenderWindow();
        initializeResourceGroups();
        setupScene();
        setupInputSystem();
        setupCEGUI();
        createFrameListener();
        startRenderLoop();
    }

    ~Application()
    {
        mInputManager->destroyInputObject(mKeyboard);
        OIS::InputManager::destroyInputSystem(mInputManager);

        delete mRenderer;
        delete mSystem;

        delete mListener;
        delete mRoot;
    }

private:
	Root *mRoot;
	OIS::Keyboard *mKeyboard;
	OIS::Mouse *mMouse;
	OIS::InputManager *mInputManager;
	CEGUI::OgreCEGUIRenderer *mRenderer;
	CEGUI::System *mSystem;
	ExitListener *mListener;

    void createRoot()
    {
        mRoot = new Root();
    }
    
    void defineResources()
    {
        String secName, typeName, archName;
        ConfigFile cf;
        cf.load("resources.cfg");

        ConfigFile::SectionIterator seci = cf.getSectionIterator();
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            ConfigFile::SettingsMultiMap *settings = seci.getNext();
            ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
                ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
            }
        }
    }
    
    void setupRenderSystem()
    {
        if (!mRoot->restoreConfig() && !mRoot->showConfigDialog())
            throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");

        //// Do not add this to the application
        //RenderSystem *rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
        //                                      // or use "OpenGL Rendering Subsystem"
        //mRoot->setRenderSystem(rs);
        //rs->setConfigOption("Full Screen", "No");
        //rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
    }
    
    void createRenderWindow()
    {
        mRoot->initialise(true, "Zombie Invasion");

        //// Do not add this to the application
        //mRoot->initialise(false);
        //HWND hWnd = 0;  // Get the hWnd of the application!
        //NameValuePairList misc;
        //misc["externalWindowHandle"] = StringConverter::toString((int)hWnd);
        //RenderWindow *win = mRoot->createRenderWindow("Main RenderWindow", 800, 600, false, &misc);
    }

    void initializeResourceGroups()
    {
        TextureManager::getSingleton().setDefaultNumMipmaps(5);
        ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    }

    void setupScene()
    {
        SceneManager *mgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE, "Default SceneManager");
		mgr->setWorldGeometry("terrain.cfg");
        Camera *cam = mgr->createCamera("Camera");
        Viewport *vp = mRoot->getAutoCreatedWindow()->addViewport(cam);
		GameWorld gameWorld(mgr);
    }

    void setupInputSystem()
    {
        size_t windowHnd = 0;
        std::ostringstream windowHndStr;
        OIS::ParamList pl;
        RenderWindow *win = mRoot->getAutoCreatedWindow();

        win->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
        mInputManager = OIS::InputManager::createInputSystem(pl);

        try
        {
            mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));
            mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
            //mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, true));
        }
        catch (const OIS::Exception &e)
        {
            throw new Exception(42, e.eText, "Application::setupInputSystem");
        }
    }

    void setupCEGUI()
    {
        SceneManager *mgr = mRoot->getSceneManager("Default SceneManager");
        RenderWindow *win = mRoot->getAutoCreatedWindow();

        // CEGUI setup
        mRenderer = new CEGUI::OgreCEGUIRenderer(win, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mgr);
        mSystem = new CEGUI::System(mRenderer);

        // Other CEGUI setup here.
		// Mouse
		CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"TaharezLookSkin.scheme");
		CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");
    }

    void createFrameListener()
    {
        mListener = new ExitListener(mKeyboard);
        mRoot->addFrameListener(mListener);

		RenderWindow *win = mRoot->getAutoCreatedWindow();
		SceneManager *mgr = mRoot->getSceneManager("Default SceneManager");
		Camera *cam = mgr->getCamera("Camera");

		CameraFrameListener *mCameraListener = new CameraFrameListener(win, cam, mgr);
		mRoot->addFrameListener(mCameraListener);

		MouseQueryListener *mMouseQueryListener = new MouseQueryListener(win, cam, mgr, mRenderer);
		mRoot->addFrameListener(mMouseQueryListener);
    }

    void startRenderLoop()
    {
        mRoot->startRendering();

        //// Do not add this to the application
        //while (mRoot->renderOneFrame())
        //{
        //    // Do some things here, like sleep for x milliseconds or perform other actions.
        //}
    }
};

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
    try
    {
        Application app;
        app.go();
    }
    catch(Exception& e)
    {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n",
            e.getFullDescription().c_str());
#endif
    }

    return 0;
}