#include "flexbox_ui_state.h"

namespace flexbox {

flexbox_ui_state flexbox_ui_state::transition() {
  flexbox_ui_state state;
  if (this->test(hover)) {
    if (this->test(unpressed)) {
      if (this->test(pressed)) {
        this->reset(unpressed);
        this->reset(pressed);
        state.set(click);
      } else {
        this->reset(unpressed);
        state.set(unpressed);
      }
    } else if (this->test(pressed)) {
      state.set(pressed);
      this->set(grabbed);
      if (this->test(motion)) {
        state.set(drag);
      }
    }
  }

  if (this->test(unpressed)) {
    this->reset(grabbed);
  }

  if (this->test(hover)) {
    this->reset(hover);
    state.set(hover);
  }

  return state;
}
}