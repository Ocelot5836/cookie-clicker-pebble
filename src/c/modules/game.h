#pragma once

#include <pebble.h>

void game_init(Window *window);

/**
 * @return Whether the screen needs to be redrawn
 */
bool game_update(Window *window);

void game_draw(Layer *layer, GContext* ctx, GBitmap* fb);

void game_draw_last(Layer *layer, GContext* ctx);

void game_free();
