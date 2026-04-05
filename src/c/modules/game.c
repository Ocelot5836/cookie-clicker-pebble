#include "game.h"
#include "../windows/main_window.h"
#include "cookie/cookie.h"

static uint64_t s_game_time;

void game_init(Window *window)
{
    cookie_load(RESOURCE_ID_COOKIE);
    s_game_time = 0;
}

bool game_update(Window *window)
{
    s_game_time++;
    return true;
}

void game_draw(Layer *layer, GContext* ctx, GBitmap *fb)
{
    GRect bounds = layer_get_bounds(layer);

    int angle = s_game_time * 10 / TPS;

    // graphics_context_set_compositing_mode(ctx, GCompOpSet);
    // graphics_context_set_antialiased(ctx, false);
    // graphics_draw_rotated_bitmap(ctx, s_cookie, GPoint(80, 80), DEG_TO_TRIGANGLE(angle), GPoint(bounds.size.w / 2, bounds.size.h / 2));

    // (sin_lookup(DEG_TO_TRIGANGLE(s_game_time * 10)) * MAX_COOKIE_SCALE) / TRIG_MAX_ANGLE
    cookie_draw(layer, fb, &GPoint(bounds.size.w >> 1, bounds.size.h >> 1), DEG_TO_TRIGANGLE(angle), 0);
}

void game_draw_last(Layer *layer, GContext* ctx)
{
}

void game_free()
{
    cookie_free();
}