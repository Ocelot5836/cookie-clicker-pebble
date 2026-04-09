#include <pebble.h>

#include "windows/main_window.h"
#include "windows/shop_window.h"
#include "modules/engine/numberformat.h"
#include "modules/storage/tempnumbers.h"

int main(void)
{
  format_init();
  tempnumbers_init();
  
  main_window_push();
  app_event_loop();
  
  main_window_free();
  shop_window_free();

  tempnumbers_free();
  format_free();
}
