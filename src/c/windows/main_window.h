#pragma once

#include <pebble.h>

#define TPS 30
#define DELTA (1000 / TPS)
// #define TIME_LOGGING 1

void main_window_push();

void main_window_free();

void main_window_set_text(char* text);
