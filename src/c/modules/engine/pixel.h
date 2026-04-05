#pragma once

#include <pebble.h>

GColor get_pixel_color(GBitmapDataRowInfo info, uint16_t x);

void set_pixel_color(GBitmapDataRowInfo info, uint16_t x, GColor color);
