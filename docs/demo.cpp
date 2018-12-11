#include "flexbox_node.h"
using namespace flexbox;

flexbox_node root(ogre_scene_mgr);

root.stylesheet.justify_content = YGJustifyCenter;
root.stylesheet.align_items     = YGAlignCenter;

flexbox_stylesheet sheet;
sheet.font_size       = 20;
sheet.width           = pixels(200);
sheet.height          = pixels(200);
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

const int window_width  = 1024;
const int window_height = 768;
root.layout_and_paint(window_width, window_height);
