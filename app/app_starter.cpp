#include "app/app_starter.h"
#include <iostream>
#include <sstream>
#include "SDL.h"

namespace flexbox {

app_starter::app_starter() {
}

app_starter::~app_starter() {
  this->deinit_sdl();
}

void app_starter::init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::stringstream ss;
    ss << "SDL could not initialize! SDL_Error: ";
    ss << std::string(SDL_GetError());
    ss << '\n';
    throw std::runtime_error(ss.str());
  }
}

void app_starter::deinit_sdl() {
  SDL_Quit();
}
}
