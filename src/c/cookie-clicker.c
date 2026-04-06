#include <pebble.h>

#include "windows/main_window.h"
#include "windows/shop_window.h"

int main(void)
{
  main_window_push();
  app_event_loop();
  main_window_free();
  shop_window_free();
}
