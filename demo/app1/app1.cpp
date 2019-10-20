#include "demo/app1/app1.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <random>
#include <thread>
#include "Compositor/OgreCompositorManager2.h"
#include "OgreArchiveManager.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsUnlit.h"
#include "OgreItem.h"
#include "OgreRenderWindow.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2_image/SDL_image.h"

#include "flexbox_node.h"

using namespace std;

using std::vector;

namespace {
float emap(float value, float from1, float to1, float from2, float to2) {
  return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}
}

namespace flexbox {

app1::app1()
  : application() {
}

app1::~app1() noexcept {
}

void app1::ogre_setup_lights() {
}

int app1::run() {

  flexbox_node root(this->ogre_scene_mgr);

  root.stylesheet.justify_content = YGJustifyCenter;
  root.stylesheet.align_items     = YGAlignCenter;

  flexbox_stylesheet sheet;
  sheet.font_size       = 10;
  sheet.width           = pixels(100);
  sheet.height          = pixels(100);
  sheet.margin_right    = 10;
  sheet.justify_content = YGJustifyCenter;
  sheet.align_items     = YGAlignCenter;
  sheet.color           = Ogre::ColourValue(0.0, 0.0, 0.0, 1.0);
  sheet.font_family     = "Futura";
  sheet.border_radius   = percent(20);

  auto square1 = root.create_child(std::shared_ptr<flexbox_canvas>::make_shared(), sheet);
  auto square2 = root.create_child(std::shared_ptr<flexbox_canvas>::make_shared(), sheet);
  square1->create_child(std::shared_ptr<flexbox_canvas_text>::make_shared("I'm on the left side"));
  square2->create_child(std::shared_ptr<flexbox_canvas_text>::make_shared("I'm on the right side"));

  square1->stylesheet.background_color     = Ogre::ColourValue(0.6627450980392157, 0.7843137254901961, 0.9568627450980393, 1.0);
  square2->stylesheet.gradient_color_start = Ogre::ColourValue(0.5764705882352941, 0.9098039215686274, 0.8274509803921568, 1.0);
  square2->stylesheet.gradient_radial      = true;

  root.layout_and_paint(this->win_w, this->win_h);

  intersections.insert(square2->computed_layout_position, square2->computed_layout_dimensions, 1);

  this->quit = 0;
  while (this->quit == 0) {
    handle_keyboard_input();
    std::this_thread::sleep_for(0.1s);

    if (application::window_dirty) {
      root.layout_mark_dirty();
      root.layout_and_paint(this->win_w, this->win_h);
      this->window_dirty = false;
    }

    this->ogre_root.renderOneFrame();
  }
  return this->quit;
}

void app1::handle_keyboard_input() {
  SDL_Event e;
  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    application::handle_input_event(e);

    // User requests quit
    if (e.type == SDL_KEYDOWN) {

      if (e.key.keysym.sym == SDLK_PLUS) {

      } else if (e.key.keysym.sym == SDLK_MINUS) {

      } else if (e.key.keysym.mod & KMOD_SHIFT) {

      } else if (e.key.keysym.sym == SDLK_c) {

      } else if (e.key.keysym.sym == SDLK_v) {
      } else if (e.key.keysym.sym == SDLK_r) {
        this->quit = 2;
      }
    } else if (e.type == SDL_KEYUP) {

    } else if (e.type == SDL_MOUSEMOTION) {
      //this->camera_controller.mouseMoved(e);

    } else if (e.type == SDL_MOUSEBUTTONUP) {

    } else if (e.type == SDL_MOUSEBUTTONDOWN) {

    } else if (e.type == SDL_WINDOWEVENT) {
      if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
        this->win_w = e.window.data1;
        this->win_h = e.window.data2;
      }
    } else {
    }
  }
}
}
