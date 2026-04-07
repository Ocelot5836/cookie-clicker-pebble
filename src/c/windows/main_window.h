#pragma once

#include <pebble.h>

#define TPS 30
#define DELTA (1000 / TPS)
#define TIME_LOGGING 0

#define MAX_TEXT_LENGTH 50
#define MAX_SUBTEXT_LENGTH 25

extern char s_text[MAX_TEXT_LENGTH + 1];
extern char s_subtext[MAX_SUBTEXT_LENGTH + 1];

void main_window_push();

void main_window_free();

void main_window_update_text();
