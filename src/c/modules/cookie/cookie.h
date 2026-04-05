#pragma once

#include <pebble.h>

#define MAX_COOKIE_SCALE 6553

void cookie_load(uint32_t resource_id);

void cookie_draw(Layer *layer, GBitmap* fb, GPoint* pos, int32_t rotation, int32_t scale);

void cookie_free();