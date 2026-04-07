#pragma once

#include <pebble.h>

#define TPS 30
#define DELTA (1000 / TPS)
// #define TIME_LOGGING 1

#define MAX_TEXT_LENGTH 50
#define MAX_SUBTEXT_LENGTH 25

void main_window_push();

void main_window_free();

void main_window_get_text(char** text, char** subtext);

void main_window_update_text();
