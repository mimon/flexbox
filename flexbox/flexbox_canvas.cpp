#include "flexbox_canvas.h"
#include <cmath>
#include <sstream>
#include "OgreHardwarePixelBuffer.h"
#include "OgreTextureManager.h"
#include "cairo.h"
#include "pango/pangocairo.h"

namespace flexbox {

flexbox_canvas::flexbox_canvas()
  : sdl_surface(nullptr, &SDL_FreeSurface),
    cairo_surface(nullptr, &cairo_surface_destroy),
    cairo_cr(nullptr, &cairo_destroy),
    pango_layout(nullptr, &g_object_unref) {
}

void flexbox_canvas::create_surface(uint32_t width, uint32_t height) {
  const unsigned int depth      = 32;
  const unsigned int r_mask     = 0x00FF0000;
  const unsigned int g_mask     = 0x0000FF00;
  const unsigned int b_mask     = 0x000000FF;
  const unsigned int alpha_mask = 0xFF000000;
  SDL_Surface *      surface    = SDL_CreateRGBSurface(
      0,
      width,
      height,
      depth,
      r_mask,
      g_mask,
      b_mask,
      alpha_mask);
  this->sdl_surface.reset(surface);

  const auto format = CAIRO_FORMAT_ARGB32;
  int        pitch  = cairo_format_stride_for_width(format, width);

  cairo_surface_t *cariro_surface = cairo_image_surface_create_for_data(
      static_cast<unsigned char *>(this->sdl_surface->pixels),
      format,
      this->sdl_surface->w,
      this->sdl_surface->h,
      pitch);
  this->cairo_surface.reset(cariro_surface);

  this->cairo_cr.reset(cairo_create(this->cairo_surface.get()));
  this->pango_layout.reset(pango_cairo_create_layout(this->cairo_cr.get()));
}

void flexbox_canvas::free_surfaces() {
  this->pango_layout.reset();
  this->cairo_cr.reset();
  this->cairo_surface.reset();
  this->sdl_surface.reset();
}

void flexbox_canvas::clear_surfaces() {
  SDL_FillRect(this->sdl_surface.get(), NULL, 0x000000);
}

Ogre::Vector2 flexbox_canvas::measure(flexbox_stylesheet const &style, float parent_width, YGMeasureMode width_mode, float parent_height, YGMeasureMode height_mode) {
  Ogre::Vector2 dim;

  switch (width_mode) {
    case YGMeasureModeUndefined:
      dim.x = style.width.value;
      break;
    case YGMeasureModeAtMost:
      dim.x = fmin(parent_width, style.width.value);
      break;
    case YGMeasureModeExactly:
      dim.x = parent_width;
  }
  switch (height_mode) {
    case YGMeasureModeUndefined:
      dim.y = style.height.value;
      break;
    case YGMeasureModeAtMost:
      dim.y = fmin(parent_height, style.height.value);
      break;
    case YGMeasureModeExactly:
      dim.y = parent_height;
  }

  return dim;
}

inline void use_solid_background(cairo_t *cr, const Ogre::ColourValue &c) {
  cairo_set_source_rgba(cr, c.r, c.g, c.b, c.a);
}

inline void use_gradient_background(cairo_t *cr, const Ogre::ColourValue &c1, const Ogre::ColourValue &c2, int width, int height) {
  std::unique_ptr<cairo_pattern_t, decltype(&cairo_pattern_destroy)> pat(nullptr, &cairo_pattern_destroy);
  pat.reset(cairo_pattern_create_radial(width / 2, height / 2, 0, width / 2, height / 2, std::min(width, height)));

  cairo_pattern_add_color_stop_rgba(pat.get(), 0, c1.r, c1.g, c1.b, c1.a);
  cairo_pattern_add_color_stop_rgba(pat.get(), 1, c2.r, c2.g, c2.b, c2.a);
  cairo_set_source(cr, pat.get());
}

void flexbox_canvas::paint_box(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions) {

  const float border_radius_normalized = style.border_radius / 100.f;

  double x             = 0.0,
         y             = 0.0,
         width         = dimensions.x,
         height        = dimensions.y,
         aspect_ratio  = 1.0,
         corner_radius = (height / 2) * border_radius_normalized;

  double radius  = corner_radius / aspect_ratio;
  double degrees = M_PI / 180.0;

  if (style.gradient_radial) {
    use_gradient_background(this->cairo_cr.get(), style.gradient_color_start, style.gradient_color_stop, width, height);
  } else {
    use_solid_background(this->cairo_cr.get(), style.background_color);
  }

  cairo_new_sub_path(this->cairo_cr.get());
  cairo_arc(this->cairo_cr.get(), x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
  cairo_arc(this->cairo_cr.get(), x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
  cairo_arc(this->cairo_cr.get(), x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
  cairo_arc(this->cairo_cr.get(), x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  cairo_close_path(this->cairo_cr.get());

  cairo_fill(this->cairo_cr.get());
}

void flexbox_canvas::paint(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions) {
  this->create_surface(dimensions.x, dimensions.y);
  this->paint_box(style, dimensions);
}

Ogre::TexturePtr flexbox_canvas::to_texture() const {
  static int          texture_count = 0;
  const auto          width         = static_cast<Ogre::Real>(this->sdl_surface->w);
  const auto          height        = static_cast<Ogre::Real>(this->sdl_surface->h);
  const Ogre::Vector2 dimensions{ width, height };
  std::stringstream   ss;

  ss << "_flexbox_node_texture" << texture_count++;
  Ogre::TextureManager &tmgr    = Ogre::TextureManager::getSingleton();
  Ogre::TexturePtr      texture = tmgr.createManual(
      ss.str(),
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D, dimensions.x, dimensions.y, 0, Ogre::PF_A8R8G8B8,
      Ogre::TU_DYNAMIC_WRITE_ONLY);
  Ogre::PixelBox pixel_box(dimensions.x, dimensions.y, 1, Ogre::PF_A8R8G8B8, this->sdl_surface->pixels);
  texture->getBuffer()->blitFromMemory(pixel_box);
  return texture;
}

flexbox_canvas_text::flexbox_canvas_text(const std::string &text_content)
  : text_content(text_content) {
}

void flexbox_canvas_text::paint_text(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions) {
  const Ogre::ColourValue &c = style.color;
  cairo_set_source_rgba(this->cairo_cr.get(), c.r, c.g, c.b, 1.0);

  PangoRectangle ink_rect;
  pango_layout_get_pixel_extents(this->pango_layout.get(), &ink_rect, nullptr);

  cairo_save(this->cairo_cr.get());
  cairo_move_to(this->cairo_cr.get(), -ink_rect.x, -ink_rect.y);
  pango_cairo_show_layout(this->cairo_cr.get(), this->pango_layout.get());
  cairo_restore(this->cairo_cr.get());
}

void flexbox_canvas_text::apply_font_styling(flexbox_stylesheet const &style) {
  pango_layout_set_wrap(this->pango_layout.get(), PANGO_WRAP_WORD);
  pango_layout_set_text(this->pango_layout.get(), this->text_content.c_str(), -1);
  pango_layout_set_alignment(this->pango_layout.get(), style.text_align);

  PangoFontDescription *desc = pango_font_description_from_string(style.font_family.c_str());
  pango_font_description_set_absolute_size(desc, style.font_size * PANGO_SCALE);
  pango_layout_set_font_description(this->pango_layout.get(), desc);
  pango_font_description_free(desc);
}

Ogre::Vector2 flexbox_canvas_text::measure(flexbox_stylesheet const &style, float parent_width, YGMeasureMode width_mode, float parent_height, YGMeasureMode height_mode) {

  flexbox_canvas_text canvas{ this->text_content };

  if (width_mode != YGMeasureModeUndefined && height_mode != YGMeasureModeUndefined) {
    canvas.create_surface(parent_width, parent_height);
  } else {
    assert(false && "This case is unhandled");
  }

  canvas.apply_font_styling(style);

  // Unless we are not allowed to be as big as we want, then
  // set limits on height and width where to break lines
  if (width_mode != YGMeasureModeUndefined) {
    pango_layout_set_width(canvas.pango_layout.get(), parent_width * PANGO_SCALE);
  }
  if (height_mode != YGMeasureModeUndefined) {
    pango_layout_set_height(canvas.pango_layout.get(), parent_height * PANGO_SCALE);
  }

  int w, h;
  pango_layout_get_pixel_size(canvas.pango_layout.get(), &w, &h);

  // Make sure we keep out size within limits
  w = std::fmin(w, parent_width);
  h = std::fmin(h, parent_height);

  return Ogre::Vector2(w, h);
}

void flexbox_canvas_text::paint(flexbox_stylesheet const &style, const Ogre::Vector2 &dimensions) {
  this->create_surface(dimensions.x, dimensions.y);
  assert(this->cairo_cr.get());
  assert(this->sdl_surface.get());

  this->paint_box(style, dimensions);

  this->apply_font_styling(style);
  pango_layout_set_width(this->pango_layout.get(), dimensions.x * PANGO_SCALE);
  pango_layout_set_height(this->pango_layout.get(), dimensions.y * PANGO_SCALE);

  if (!this->text_content.empty()) {
    paint_text(style, dimensions);
  }
}
}
