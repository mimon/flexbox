#include "flexbox/flexbox_node.h"

namespace flexbox {
class widget_slider {
  public:
  widget_slider(std::shared_ptr<flexbox_node> parent_container, const std::string &label = "N/A");

  void move_traveler(pixels displacement);

  std::shared_ptr<flexbox_node>        root;
  std::shared_ptr<flexbox_node>        slider_container;
  std::shared_ptr<flexbox_node>        slider_label_container;
  std::shared_ptr<flexbox_node>        slider_label;
  std::shared_ptr<flexbox_node>        slider_value_label;
  std::shared_ptr<flexbox_canvas_text> slider_label_text;
  std::shared_ptr<flexbox_canvas_text> slider_value_label_text;
  std::shared_ptr<flexbox_node>        traveler_rail_container;
  std::shared_ptr<flexbox_node>        traveler_rail;
  std::shared_ptr<flexbox_node>        traveler_node;

  float slider_value = 1.0f;
};
}