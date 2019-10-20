#include <vector>
#include "app/application.h"
#include "flexbox_ui_intersection.h"
#include "flexbox_ui_state.h"

namespace flexbox {
class app1 : public flexbox::application {
  public:
  app1();
  ~app1() noexcept;

  virtual void ogre_setup_lights();

  void handle_keyboard_input();

  void handle_user_events();

  int run();

  protected:
  float                                step_size, simulation_speed, accumulator, simulation_threshold;
  flexbox_ui_intersection<std::size_t> intersections;
  std::vector<flexbox_ui_state>        ui_states;
};
}