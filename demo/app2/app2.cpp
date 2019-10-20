#include "demo/app2/app2.h"
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
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_syswm.h"
#include "flexbox_node.h"
#include "fmt/format.h"

using namespace std;

template <class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
  return std::max(lo, std::min(v, hi));
}

using std::vector;

namespace {
float emap(float value, float from1, float to1, float from2, float to2) {
  return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
}
}

namespace flexbox {

app2::app2()
  : application() {
}

app2::~app2() noexcept {
}

void app2::ogre_setup_lights() {
}

int app2::run() {

  root.reset(new flexbox_node(this->ogre_scene_mgr));

  root->stylesheet.justify_content = YGJustifyCenter;
  root->stylesheet.align_items     = YGAlignCenter;
  root->stylesheet.flex_direction  = YGFlexDirectionColumn;

  flexbox_stylesheet container_sheet;
  container_sheet.set_margin(pixels(40), 0, pixels(40));
  container_sheet.font_size      = 10;
  container_sheet.width          = pixels(500);
  container_sheet.height         = pixels(15);
  container_sheet.font_family    = "Futura";
  container_sheet.flex_direction = YGFlexDirectionColumn;
  container_sheet.flex_shrink    = 1.0;

  flexbox_stylesheet rail_container_sheet;
  rail_container_sheet.width           = percent(100);
  rail_container_sheet.height          = pixels(25);
  rail_container_sheet.align_items     = YGAlignCenter;
  rail_container_sheet.justify_content = YGJustifyFlexStart;

  flexbox_stylesheet rail_sheet;
  rail_sheet.font_size     = 10;
  rail_sheet.width         = percent(100);
  rail_sheet.height        = pixels(7);
  rail_sheet.color         = Ogre::ColourValue(0.0, 0.0, 0.0, 1.0);
  rail_sheet.font_family   = "Rail";
  rail_sheet.border_radius = percent(20);

  flexbox_stylesheet label_container_sheet;
  label_container_sheet.justify_content = YGJustifySpaceBetween;
  label_container_sheet.flex_direction  = YGFlexDirectionRow;
  label_container_sheet.flex_grow       = 1.0;
  label_container_sheet.font_family     = "label container";
  label_container_sheet.width           = percent(100);

  flexbox_stylesheet label_sheet;
  label_sheet.font_family = "Courier New";

  slider_container = root->create_child(std::shared_ptr<flexbox_canvas>::make_shared(), container_sheet);

  slider_label_container  = slider_container->create_child(std::shared_ptr<flexbox_canvas>::make_shared(), label_container_sheet);
  slider_label_text       = shared_ptr<flexbox_canvas_text>::make_shared("N/A");
  slider_value_label_text = shared_ptr<flexbox_canvas_text>::make_shared("N/A");
  slider_label            = slider_label_container->create_child(slider_label_text, label_sheet);
  slider_value_label      = slider_label_container->create_child(slider_value_label_text, label_sheet);

  traveler_rail_container = slider_container->create_child(std::shared_ptr<flexbox_canvas>::make_shared(), rail_container_sheet);

  auto traveler                              = std::shared_ptr<flexbox_canvas>::make_shared();
  traveler_node                              = traveler_rail_container->create_child(traveler);
  traveler_node->stylesheet.width            = pixels(25);
  traveler_node->stylesheet.height           = pixels(25);
  traveler_node->stylesheet.border_radius    = percent(100);
  traveler_node->stylesheet.background_color = Ogre::ColourValue(0.6627450980392157, 0.7843137254901961, 0.9568627450980393, 1.0);
  ;
  traveler_node->stylesheet.position_type = YGPositionTypeAbsolute;

  traveler_rail                              = traveler_rail_container->create_child(std::shared_ptr<flexbox_canvas>::make_shared(), rail_sheet);
  traveler_rail->stylesheet.background_color = Ogre::ColourValue(0.5764705882352941, 0.9098039215686274, 0.8274509803921568, 1.0);

  root->layout_and_paint(this->win_w, this->win_h);

  // intersections.insert(square2->computed_layout_position, square2->computed_layout_dimensions, 1);

  sliders.emplace_back(root->create_child(), "Metallness");
  slider_moveable_area.push_back(flexbox_ui_state());
  slider_traveler_state.push_back(flexbox_ui_state());
  sliders.emplace_back(root->create_child(), "Specular");
  slider_moveable_area.push_back(flexbox_ui_state());
  slider_traveler_state.push_back(flexbox_ui_state());

  this->quit = 0;
  while (this->quit == 0) {
    handle_keyboard_input();
    handle_ui_events();
    std::this_thread::sleep_for(0.01666s);

    if (application::window_dirty) {
      root->layout_mark_dirty();
      root->layout_and_paint(this->win_w, this->win_h);
      this->window_dirty = false;
    }

    this->ogre_root.renderOneFrame();
  }
  return this->quit;
}

void app2::handle_ui_events() {
  // flexbox_ui_state new_state = ui_state.transition();

  int x_rel, y_rel;
  int x_abs, y_abs;
  SDL_GetMouseState(&x_abs, &y_abs);
  SDL_GetRelativeMouseState(&x_rel, &y_rel);
  {
    for (int i = 0; i < sliders.size(); ++i) {
      auto       clickstate          = slider_traveler_state[i].transition();
      const bool pressed_on_traveler = slider_traveler_state[i].test(flexbox_ui_state_flag::grabbed);
      if (slider_moveable_area[i].test(flexbox_ui_state_flag::hover) && pressed_on_traveler) {

        fmt::print("Hover!");
        sliders[i].move_traveler(x_rel);
        root->layout_and_paint(this->win_w, this->win_h);
      }
    }
  }
  if (ui_state.test(flexbox_ui_state_flag::pressed)) {
    std::cout << "pressed" << std::endl;

    auto const pos    = traveler_rail->computed_layout_position;
    auto const dim    = traveler_rail->computed_layout_dimensions;
    const bool inside = x_abs < (pos.x + dim.x) and x_abs > pos.x;
    if (inside) {
      // move_traveler(x_rel);
    }
  }
}

void app2::move_traveler(int x) {
  float v                                      = traveler_node->stylesheet.left + x;
  v                                            = clamp(v, 0.0f, traveler_rail->computed_layout_dimensions.x);
  slider_value                                 = 1.0 - v / traveler_rail->computed_layout_dimensions.x;
  traveler_node->stylesheet.left               = pixels(v);
  traveler_rail->stylesheet.background_color.r = slider_value * 0.5764705882352941;
  traveler_rail->layout_mark_dirty();

  slider_value_label_text->text_content = fmt::format("{:1.2f}", 1.0 - slider_value);
  slider_value_label_text->sdl_surface.reset();
  slider_value_label->layout_mark_dirty();
  root->layout_and_paint(this->win_w, this->win_h);
}

void app2::handle_keyboard_input() {
  SDL_Event e;
  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    application::handle_input_event(e);

    // User requests quit
    if (e.type == SDL_KEYDOWN) {

      if (e.key.keysym.sym == SDLK_PLUS) {

      } else if (e.key.keysym.sym == SDLK_MINUS) {

      } else if (e.key.keysym.mod & KMOD_SHIFT) {

      } else if (e.key.keysym.sym == SDLK_RIGHT) {
        traveler_node->stylesheet.left += 1;
      } else if (e.key.keysym.sym == SDLK_LEFT) {
        traveler_node->stylesheet.left -= 1;
      } else if (e.key.keysym.sym == SDLK_v) {
      } else if (e.key.keysym.sym == SDLK_r) {
        this->quit = 2;
      }
    } else if (e.type == SDL_KEYUP) {

    } else if (e.type == SDL_MOUSEMOTION) {
      ui_state.set(flexbox_ui_state_flag::motion);

      flexbox_ui_intersection<std::size_t> intersections;
      flexbox_ui_intersection<std::size_t> drag_area;

      //intersections.insert(traveler_node->computed_layout_position, traveler_node->computed_layout_dimensions, 0);

      for (size_t i = 0; i < sliders.size(); ++i) {
        const widget_slider &slider = sliders[i];
        intersections.insert(slider.traveler_node->computed_layout_position, slider.traveler_node->computed_layout_dimensions, i);
        drag_area.insert(slider.root->computed_layout_position, slider.root->computed_layout_dimensions, i);
      }

      for (auto &ui_state : slider_moveable_area) {
        ui_state.reset(flexbox_ui_state_flag::hover);
      }

      Ogre::Vector2 ray(e.motion.x, e.motion.y);
      auto          v          = intersections.get_intersecting_intersectables(ray);
      auto          drag_areas = drag_area.get_intersecting_intersectables(ray);

      for (auto idx : v) {
        slider_traveler_state[idx].set(flexbox_ui_state_flag::hover);
      }

      for (auto idx : drag_areas) {
        slider_moveable_area[idx].set(flexbox_ui_state_flag::hover);
      }

      if (v.size() > 0) {
        SDL_Cursor *cursor;
        cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        SDL_SetCursor(cursor);
        ui_state.set(flexbox_ui_state_flag::hover);
        hover = true;
      } else {
        // ui_state.reset(flexbox_ui_state_flag::hover);
        hover = false;
        SDL_Cursor *cursor;
        cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        SDL_SetCursor(cursor);
      }

      if (drag) {
        // move_traveler(e.motion.xrel);
      }

    } else if (e.type == SDL_MOUSEBUTTONUP) {
      ui_state.set(flexbox_ui_state_flag::unpressed);
      ui_state.reset(flexbox_ui_state_flag::pressed);
      drag = false;
      for (int i = 0; i < slider_traveler_state.size(); ++i) {
        slider_traveler_state[i].set(flexbox_ui_state_flag::unpressed);
      }
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
      ui_state.set(flexbox_ui_state_flag::pressed);
      for (int i = 0; i < slider_traveler_state.size(); ++i) {
        slider_traveler_state[i].set(flexbox_ui_state_flag::pressed);
      }
      drag = hover;
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
