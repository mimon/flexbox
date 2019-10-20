#pragma once
#include <memory>

namespace flexbox {
class app_starter {
  template <typename T>
  class deleter {
    public:
    void operator()(T *obj) {
      obj->release();
    }
  };

  public:
  app_starter();
  ~app_starter();

  void init_sdl();

  void deinit_sdl();
};
}