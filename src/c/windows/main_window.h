#pragma once

#include <pebble.h>

#define TPS 25
#define DELTA (1000 / TPS)
#define TIME_LOGGING

void main_window_push();
