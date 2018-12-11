#include <bitset>

namespace flexbox {
enum flexbox_ui_state_flag {
  none,
  pressed,
  unpressed,
  hover,
  click,
  drag,
  dbl_click,
  number_of_flags
};
typedef std::bitset<number_of_flags> state_flags;
class flexbox_ui_state : public state_flags {
  public:
  flexbox_ui_state()
    : state_flags(flexbox_ui_state_flag::none) {
  }

  flexbox_ui_state transition();
};
}