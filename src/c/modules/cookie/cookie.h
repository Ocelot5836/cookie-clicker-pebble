#pragma once

#include <pebble.h>

#define MAX_COOKIE_SCALE 255
#define COOKIE_SCALE_INCREMENT (MAX_COOKIE_SCALE >> 4)

void cookie_load(uint32_t resource_id);

void cookie_draw(Layer *layer, GBitmap* fb, GPoint* pos, int32_t rotation, uint8_t scale);

void cookie_free();