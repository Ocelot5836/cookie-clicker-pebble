#pragma once

#include <pebble.h>
#include "buildings/buildings.h"

#define GAME_MAX_ANIMATION_TIME (3 * TPS)

void game_init(Window *window);

void game_init_resources();

void game_free_resources();

void game_update(Window *window, GRect bounds);

void game_draw_first(Layer *layer, GContext *ctx);

void game_draw(Layer *layer, GContext *ctx, GBitmap *fb);

void game_click();

uint8_t game_purchase(BuildingType building, uint8_t count);

uint8_t* game_get_building_counts();

BigInt_t *game_get_cookie_count();

void game_free();
