#include <vector>
#include "app/application.h"
#include "flexbox_node.h"
#include "flexbox_ui_intersection.h"
#include "flexbox_ui_state.h"
#include "widgets/widget_slider.h"

namespace flexbox {
class app2 : public flexbox::application {
  public:
  app2();
  ~app2() noexcept;

  virtual void ogre_setup_lights();

  void handle_keyboard_input();

  void handle_user_events();

  void handle_ui_events();

  void move_traveler(int x);

  int run();

  protected:
  float                                step_size, simulation_speed, accumulator, simulation_threshold;
  flexbox_ui_intersection<std::size_t> intersections;
  flexbox_ui_state                     ui_state;
  std::unique_ptr<flexbox_node>        root;
  std::shared_ptr<flexbox_node>        slider_container;
  std::shared_ptr<flexbox_node>        slider_label_container;
  std::shared_ptr<flexbox_node>        slider_label;
  std::shared_ptr<flexbox_node>        slider_value_label;
  std::shared_ptr<flexbox_canvas_text> slider_label_text;
  std::shared_ptr<flexbox_canvas_text> slider_value_label_text;
  std::shared_ptr<flexbox_node>        traveler_rail_container;
  std::shared_ptr<flexbox_node>        traveler_rail;
  std::shared_ptr<flexbox_node>        traveler_node;

  std::vector<flexbox_ui_state> slider_moveable_area;
  std::vector<flexbox_ui_state> slider_traveler_state;
  std::vector<widget_slider>    sliders;

  bool  hover        = false;
  bool  drag         = false;
  float slider_value = 1.0f;
};
}