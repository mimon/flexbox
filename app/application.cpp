#include "app/application.h"
#include "Compositor/OgreCompositorManager2.h"
#include "OgreArchiveManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsUnlit.h"
#include "OgreHlmsUnlitDatablock.h"
#include "OgreItem.h"
#include "OgrePixelBox.h"
#include "OgreRenderWindow.h"
#include "OgreTextureManager.h"
#include "SDL.h"
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_image.h"
#include "app/osx_utils.h"

namespace flexbox {

static std::string executable_directory_;
static std::string resource_path_;

application::application()
  : window(nullptr, &SDL_DestroyWindow) {
}

void application::init_sdl(int win_w, int win_h) {
  this->win_w = win_w;
  this->win_h = win_h;

  // Create window
  window.reset(SDL_CreateWindow("Flexbox", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, win_w,
                                win_h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
  if (!window) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
  }
}

void application::init() {
  this->init_sdl(1024, 768);
  this->init_ogre();
}

void application::init_ogre() {
  this->ogre_root.installPlugin(&this->ogre_metal_plugin);

  Ogre::RenderSystem *rs =
      this->ogre_root.getRenderSystemByName("Metal Rendering Subsystem");
  this->ogre_root.setRenderSystem(rs);

  this->ogre_root.initialise(false);

  //Get the native whnd
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);

  if (SDL_GetWindowWMInfo(this->window.get(), &wmInfo) == SDL_FALSE) {
    // error
  }

  Ogre::String            winHandle;
  Ogre::NameValuePairList params;
  if (wmInfo.subsystem == SDL_SYSWM_COCOA) {
    params.insert(std::make_pair("gamma", "true"));
    params.insert(std::make_pair("macAPI", "cocoa"));
    params.insert(std::make_pair("macAPICocoaUseNSView", "true"));
    winHandle = Ogre::StringConverter::toString(window_content_view_handle(wmInfo));
    params.insert(std::make_pair("parentWindowHandle", winHandle));
    this->ogre_render_window = this->ogre_root.createRenderWindow("GUIOGREX 3D", this->win_w, this->win_h, false, &params);
  } else {
    // error
  }

  this->ogre_init_scene_mgr();
  this->ogre_init_hlms();
  this->ogre_init_resources();
  this->ogre_setup_compositor();
}

void application::ogre_init_scene_mgr() {
  this->ogre_scene_mgr   = this->ogre_root.createSceneManager(Ogre::ST_GENERIC, 1, Ogre::INSTANCING_CULLING_SINGLETHREAD);
  this->ogre_camera      = this->ogre_scene_mgr->createCamera("MainCamera");
  this->ogre_camera_node = this->ogre_scene_mgr->getRootSceneNode()->createChildSceneNode();
  // this->ogre_camera->setOrientation(Ogre::Quaternion(Ogre::Radian(M_PI / 2), Ogre::Vector3::UNIT_X));
  this->ogre_camera->setNearClipDistance(0.2f);
  this->ogre_camera->setFarClipDistance(1000.0f);
  this->ogre_camera->setAutoAspectRatio(true);
  this->ogre_camera->detachFromParent();
  this->ogre_camera_node->attachObject(this->ogre_camera);
  this->ogre_camera_node->setPosition(0, -2, 0);

  this->ogre_setup_lights();
}

void application::ogre_setup_lights() {
  Ogre::Light *    light      = this->ogre_scene_mgr->createLight();
  Ogre::Light *    light2     = this->ogre_scene_mgr->createLight();
  Ogre::SceneNode *light_node = this->ogre_scene_mgr->getRootSceneNode()->createChildSceneNode();
  light_node->setPosition(0, 10, 0);
  light_node->attachObject(light);
  light_node->attachObject(light2);
  light->setPowerScale(0.2);
  light->setType(Ogre::Light::LT_DIRECTIONAL);
  light2->setType(Ogre::Light::LT_SPOTLIGHT);
  light->setDirection(Ogre::Vector3(0, -1, 0).normalisedCopy());
  this->ogre_scene_mgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.5f, 0.7f) * 0.1f * 0.75f,
                                        Ogre::ColourValue(0.6f, 0.45f, 0.3f) * 0.065f * 0.75f,
                                        -light->getDirection() + Ogre::Vector3::UNIT_Y * 0.2f);
}

void application::ogre_setup_compositor() {
  Ogre::CompositorManager2 *compositorManager = this->ogre_root.getCompositorManager2();

  const Ogre::String workspaceName("GUIOGREX Workspace");
  if (!compositorManager->hasWorkspaceDefinition(workspaceName)) {
    compositorManager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue(1.0f, 1.0f, 1.0f),
                                               Ogre::IdString());
  }

  this->ogre_workspace = compositorManager->addWorkspace(this->ogre_scene_mgr, this->ogre_render_window, this->ogre_camera, workspaceName, true);
}

void application::ogre_init_resources() {
  auto &r = Ogre::ResourceGroupManager::getSingleton();

  std::string p = executable_directory();

  if (!this->app_name.empty()) {
    r.addResourceLocation(Ogre::String(p + this->app_name), "FileSystem", "GUIOGREX", true);
  }

  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(true);
}

void application::ogre_init_hlms() {
  Ogre::String rootHlmsFolder = this->executable_directory() + "/Data/Common/";

  Ogre::HlmsUnlit *hlmsUnlit = 0;

  //For retrieval of the paths to the different folders needed
  Ogre::String                       mainFolderPath;
  Ogre::StringVector                 libraryFoldersPaths;
  Ogre::StringVector::const_iterator libraryFolderPathIt;
  Ogre::StringVector::const_iterator libraryFolderPathEn;

  Ogre::ArchiveManager &archiveManager = Ogre::ArchiveManager::getSingleton();

  {
    //Create & Register HlmsUnlit
    //Get the path to all the subdirectories used by HlmsUnlit
    Ogre::HlmsUnlit::getDefaultPaths(mainFolderPath, libraryFoldersPaths);
    Ogre::Archive *archiveUnlit = archiveManager.load(rootHlmsFolder + mainFolderPath,
                                                      "FileSystem", true);
    Ogre::ArchiveVec archiveUnlitLibraryFolders;
    libraryFolderPathIt = libraryFoldersPaths.begin();
    libraryFolderPathEn = libraryFoldersPaths.end();
    while (libraryFolderPathIt != libraryFolderPathEn) {
      Ogre::Archive *archiveLibrary =
          archiveManager.load(rootHlmsFolder + *libraryFolderPathIt, "FileSystem", true);
      archiveUnlitLibraryFolders.push_back(archiveLibrary);
      ++libraryFolderPathIt;
    }

    //Create and register the unlit Hlms
    hlmsUnlit = OGRE_NEW Ogre::HlmsUnlit(archiveUnlit, &archiveUnlitLibraryFolders);
    Ogre::Root::getSingleton().getHlmsManager()->registerHlms(hlmsUnlit);
  }
}

void application::handle_input_event(SDL_Event &e) {
  // User requests quit ?
  this->quit = (e.type == SDL_QUIT);

  if (e.type == SDL_WINDOWEVENT) {
    if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
      this->win_w        = e.window.data1;
      this->win_h        = e.window.data2;
      this->window_dirty = true;
    }
  }
}

const std::string &application::executable_directory() {
#ifdef _WIN32
// 32-bit and 64-bit
#ifdef _WIN64
// 64-bit only
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
  CFBundleRef mainBundle   = CFBundleGetMainBundle();
  CFURLRef    resourcesURL = CFBundleCopyBundleURL(mainBundle);
  char        path[PATH_MAX];
  if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
    // error!
  } else {
    executable_directory_ = std::string(path);
  }
  CFRelease(resourcesURL);

  return executable_directory_;

#else
#error "Unknown Apple platform"
#endif
#elif __linux__
// linux
#elif __unix__  // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#error "Unknown compiler"
#endif
  return resource_path_;
}
}
