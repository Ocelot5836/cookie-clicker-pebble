#include <pebble.h>

#include "windows/main_window.h"

int main(void)
{
  main_window_push();
  app_event_loop();
}
