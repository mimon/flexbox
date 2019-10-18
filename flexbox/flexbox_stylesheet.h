#pragma once
#include <bitset>
#include "OgreColourValue.h"
#include "OgreVector2.h"
#include "SDL.h"
#include "Yoga.h"
#include "pango/pangocairo.h"

namespace flexbox {

typedef int   pixels;
typedef int   points;
typedef float percent;

struct pixels_or_percent {
  bool  is_pixel = true;
  bool  is_none  = true;
  float value;

  pixels_or_percent(pixels px)
    : is_pixel(true), value(px) {
  }
  pixels_or_percent(percent pct)
    : is_pixel(false), value(pct) {
  }
};

class flexbox_stylesheet {
  public:
  void set_padding(pixels top_right_bottom_left);
  void set_padding(pixels top_bottom, pixels right_left);
  void set_padding(pixels top, pixels right_left, pixels bottom);
  void set_padding(pixels top, pixels right, pixels bottom, pixels left);

  void set_margin(pixels top_right_bottom_left);
  void set_margin(pixels top_bottom, pixels right_left);
  void set_margin(pixels top, pixels right_left, pixels bottom);
  void set_margin(pixels top, pixels right, pixels bottom, pixels left);

  pixels top  = 0;
  pixels left = 0;

  pixels_or_percent width  = -1;
  pixels_or_percent height = -1;

  pixels margin_top    = 0;
  pixels margin_right  = 0;
  pixels margin_bottom = 0;
  pixels margin_left   = 0;

  pixels padding_top    = 0;
  pixels padding_right  = 0;
  pixels padding_bottom = 0;
  pixels padding_left   = 0;

  pixels            font_size            = 12;
  std::string       font_family          = "Helvetica";
  Ogre::ColourValue background_color     = Ogre::ColourValue::ZERO;
  Ogre::ColourValue color                = Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0);
  PangoAlignment    text_align           = PANGO_ALIGN_LEFT;
  YGPositionType    position_type        = YGPositionTypeRelative;
  percent           border_radius        = 0;
  bool              visible              = true;
  YGFlexDirection   flex_direction       = YGFlexDirectionRow;
  float             flex_grow            = 0.0;
  float             flex_shrink          = 0.0;
  YGJustify         justify_content      = YGJustifyFlexStart;
  YGAlign           align_items          = YGAlignFlexStart;
  Ogre::ColourValue gradient_color_start = Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f);
  Ogre::ColourValue gradient_color_stop  = Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f);
  bool              gradient_radial      = false;
};

flexbox_stylesheet create_and_inherit(const flexbox_stylesheet &inherit_from);
}
