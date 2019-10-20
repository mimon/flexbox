#include "app/app_starter.h"
#include "demo/app1/app1.h"

namespace flexbox {
int main_run(flexbox::application *app, int argc, char *argv[]);
}

int main(int argc, char *argv[]) {
  flexbox::app_starter app_starter;
  app_starter.init_sdl();

  flexbox::app1 app;
  app.init();
  app.run();

  return 0;
}
