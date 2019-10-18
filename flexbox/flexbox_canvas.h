#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "OgreVector2.h"
#include "SDL.h"
#include "cairo.h"

#include "flexbox_stylesheet.h"

typedef std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>          unique_sdl_surface;
typedef std::unique_ptr<cairo_surface_t, decltype(&cairo_surface_finish)> unique_cairo_surface;
typedef std::unique_ptr<cairo_t, decltype(&cairo_destroy)>                unique_cairo_cr;
typedef std::unique_ptr<PangoLayout, decltype(&g_object_unref)>           unique_pango_layout;
typedef std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>       unique_sdl_texture;

namespace flexbox {

/// Holds data needed to rener a SDL/Cairo surface
class flexbox_canvas {
  public:
  flexbox_canvas();
  virtual ~flexbox_canvas(){};

  virtual void paint(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions);

  virtual void paint_box(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions);

  virtual Ogre::Vector2 measure(flexbox_stylesheet const &style, float parent_width, YGMeasureMode width_mode, float parent_height, YGMeasureMode height_mode);

  void create_surface(uint32_t width, uint32_t height);

  void free_surfaces();

  void clear_surfaces();

  Ogre::TexturePtr to_texture() const;

  unique_sdl_surface   sdl_surface;
  unique_cairo_surface cairo_surface;
  unique_cairo_cr      cairo_cr;
  unique_pango_layout  pango_layout;
};

///  What does this class do?
class flexbox_canvas_text : public flexbox_canvas {
  public:
  flexbox_canvas_text(const std::string &text_content);
  virtual Ogre::Vector2 measure(flexbox_stylesheet const &style, float parent_width, YGMeasureMode width_mode, float parent_height, YGMeasureMode height_mode);

  virtual void paint(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions);

  void paint_text(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions);

  void apply_font_styling(flexbox_stylesheet const &style);

  std::string text_content;
};
}