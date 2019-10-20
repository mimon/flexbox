#include "widget_slider.h"
#include "fmt/format.h"

using namespace std;

template <class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
  return std::max(lo, std::min(v, hi));
}

namespace flexbox {
widget_slider::widget_slider(std::shared_ptr<flexbox_node> parent_container, const std::string &label)
  : root(parent_container) {
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
  label_sheet.font_size   = 15;

  slider_container = root->create_child(std::shared_ptr<flexbox_canvas>::make_shared(), container_sheet);

  slider_label_container  = slider_container->create_child(std::shared_ptr<flexbox_canvas>::make_shared(), label_container_sheet);
  slider_label_text       = shared_ptr<flexbox_canvas_text>::make_shared(label);
  slider_value_label_text = shared_ptr<flexbox_canvas_text>::make_shared(fmt::format("{:1.2f}", slider_value));
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
}

void widget_slider::move_traveler(pixels displacement) {
  float v                                      = traveler_node->stylesheet.left + displacement;
  v                                            = clamp(v, 0.0f, traveler_rail->computed_layout_dimensions.x);
  slider_value                                 = 1.0 - v / traveler_rail->computed_layout_dimensions.x;
  traveler_node->stylesheet.left               = pixels(v);
  traveler_rail->stylesheet.background_color.r = slider_value * 0.5764705882352941;
  traveler_rail->layout_mark_dirty();

  slider_value_label_text->text_content = fmt::format("{:1.2f}", 1.0 - slider_value);
  slider_value_label_text->sdl_surface.reset();
  slider_value_label->layout_mark_dirty();
}
}