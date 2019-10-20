#pragma once
#include <memory>
#include "OgreCamera.h"
#include "OgreMetalPlugin.h"
#include "OgrePrerequisites.h"
#include "OgreRoot.h"
#include "SDL.h"

struct SDL_Window;
struct SDL_Renderer;

namespace flexbox {

///  What does this class do?
class application {
  typedef std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> unique_sdl_window;

  public:
  application();
  application(const application &) = delete;
  virtual ~application()           = default;

  void init();

  virtual void init_sdl(int, int);

  virtual void init_ogre();

  virtual void ogre_setup_compositor();

  virtual void ogre_init_scene_mgr();

  virtual void ogre_setup_lights();

  virtual void ogre_init_hlms();

  virtual void ogre_init_resources();

  const std::string &executable_directory();

  virtual void handle_input_event(SDL_Event &e);

  std::string       app_name;
  float             step_size, simulation_speed, accumulator, simulation_threshold;
  int               win_h        = 768;
  int               win_w        = 1024;
  bool              quit         = false;
  bool              window_dirty = true;
  unique_sdl_window window;

  Ogre::MetalPlugin          ogre_metal_plugin;
  Ogre::Root                 ogre_root;
  Ogre::RenderWindow *       ogre_render_window;
  Ogre::SceneManager *       ogre_scene_mgr;
  Ogre::CompositorWorkspace *ogre_workspace;
  Ogre::Camera *             ogre_camera;
  Ogre::SceneNode *          ogre_camera_node;
};
}
