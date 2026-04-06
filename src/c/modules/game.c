#include "game.h"
#include "../windows/main_window.h"
#include "cookie/cookie.h"
#include "storage/storage.h"
#include "engine/numberformat.h"
#include <math.h>

#define MAX_TEXT_LENGTH 50

static uint64_t s_game_time;
static uint8_t s_animation_time;
static BigInt_t *s_cookie_count;
static char s_counter[MAX_TEXT_LENGTH + 1];

static uint8_t get_scale(uint8_t x)
{
    return x == 0
               ? 0
           : x == 1
               ? COOKIE_SCALE_INCREMENT
               : (pow(2.0, -10.0 * x / GAME_MAX_ANIMATION_TIME) * (double)sin_lookup(DEG_TO_TRIGANGLE(x * 1200 / GAME_MAX_ANIMATION_TIME - 90)) / TRIG_MAX_ANGLE + 1.0) * COOKIE_SCALE_INCREMENT;
}

static void update_text()
{
    format_cookie_number(s_cookie_count, MAX_TEXT_LENGTH, s_counter);
    main_window_set_text(s_counter);
}

void game_init(Window *window)
{
    s_game_time = 0;
    s_animation_time = 0;
}

void game_init_resources()
{
    cookie_load(RESOURCE_ID_COOKIE);
    s_cookie_count = calloc(BigIntWordSize, BigIntWordSize * COOKIE_COUNTER_WORDS);
    storage_read(s_cookie_count);

    update_text();
}

void game_free_resources()
{
    free(s_cookie_count);
    cookie_free();
}

bool game_update(Window *window)
{
    s_game_time++;
    if (s_animation_time > 0)
    {
        s_animation_time--;
    }

    if ((s_game_time % TPS) == 0)
    {
        storage_write(s_cookie_count);
        update_text();
    }

    return true;
}

void game_draw(Layer *layer, GContext *ctx, GBitmap *fb)
{
    GRect bounds = layer_get_bounds(layer);

    int angle = s_game_time * 10 / TPS;

    // graphics_context_set_compositing_mode(ctx, GCompOpSet);
    // graphics_context_set_antialiased(ctx, false);
    // graphics_draw_rotated_bitmap(ctx, s_cookie, GPoint(80, 80), DEG_TO_TRIGANGLE(angle), GPoint(bounds.size.w / 2, bounds.size.h / 2));

    // (sin_lookup(DEG_TO_TRIGANGLE(s_game_time * 10)) * MAX_COOKIE_SCALE) / TRIG_MAX_ANGLE
    cookie_draw(layer, fb, &GPoint(bounds.size.w >> 1, bounds.size.h >> 1), DEG_TO_TRIGANGLE(angle), get_scale(GAME_MAX_ANIMATION_TIME - s_animation_time));
}

void game_draw_last(Layer *layer, GContext *ctx)
{
}

void game_click()
{
    s_animation_time = GAME_MAX_ANIMATION_TIME;
    BigInt_inc(COOKIE_COUNTER_WORDS, s_cookie_count);
    update_text();
}

void game_free()
{
}