#pragma once

#include <pebble.h>
#include "../windows/main_window.h"

#define GAME_MAX_ANIMATION_TIME (3 * TPS)

void game_init(Window *window);

void game_init_resources();

void game_free_resources();

/**
 * @return Whether the screen needs to be redrawn
 */
bool game_update(Window *window);

void game_draw(Layer *layer, GContext* ctx, GBitmap* fb);

void game_draw_last(Layer *layer, GContext* ctx);

void game_click();

void game_free();
