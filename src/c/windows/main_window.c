#include "main_window.h"
#include "../modules/game.h"

// #define TIME_LOGGING

static Window *s_window;
static Layer *s_render_layer;
static AppTimer *s_next_frame_timer;
static bool s_screen_dirty;

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
}

static void click_config_provider(void *context)
{
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void next_frame_handler(void *context)
{
    // Run
    uint16_t startMs = time_ms(NULL, NULL);
    s_screen_dirty |= game_update(s_window);
    uint16_t endMs = time_ms(NULL, NULL);

    // Draw the next frame
    if (s_screen_dirty)
    {
        layer_mark_dirty(window_get_root_layer(s_window));
    }

    uint16_t delta = endMs - startMs;
    if (delta >= DELTA)
    {
        s_next_frame_timer = app_timer_register(0, next_frame_handler, NULL);
    }
    else
    {
        s_next_frame_timer = app_timer_register(DELTA - delta, next_frame_handler, NULL);
    }

#ifdef TIME_LOGGING
    if (delta > 0)
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "Took %dms to update", delta);
    }
#endif
}

static void render_handler(Layer *layer, GContext *ctx)
{
    if (!s_screen_dirty)
    {
        return;
    }

    s_screen_dirty = false;

    GBitmap *fb = graphics_capture_frame_buffer(ctx);

#ifdef TIME_LOGGING
    uint16_t startDrawMs = time_ms(NULL, NULL);
#endif
    game_draw(layer, ctx, fb);
#ifdef TIME_LOGGING
    uint16_t endDrawMs = time_ms(NULL, NULL);
#endif

    graphics_release_frame_buffer(ctx, fb);

#ifdef TIME_LOGGING
    uint16_t startDrawLastMs = time_ms(NULL, NULL);
#endif
    game_draw_last(layer, ctx);
#ifdef TIME_LOGGING
    uint16_t endDrawLastMs = time_ms(NULL, NULL);
#endif

#ifdef TIME_LOGGING
    uint16_t delta = (endDrawMs - startDrawMs) + (endDrawLastMs - startDrawLastMs);
    if (delta > 0)
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "Took %dms (%dms, %dms) to draw", delta, endDrawLastMs, startDrawLastMs);
    }
#endif
}

static void window_appear(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "APPEAR");
    s_screen_dirty = true;
    s_next_frame_timer = app_timer_register(DELTA, next_frame_handler, NULL);
}

static void window_disappear(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "DISAPPEAR");
    app_timer_cancel(s_next_frame_timer);
    s_next_frame_timer = NULL;
}

static void window_load(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "LOAD");
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    game_init(window);

    s_render_layer = layer_create(bounds);
    layer_set_update_proc(s_render_layer, render_handler);
    layer_add_child(window_layer, s_render_layer);

    s_next_frame_timer = app_timer_register(DELTA, next_frame_handler, NULL);
}

static void window_unload(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "UNLOAD");

    layer_destroy(s_render_layer);

    window_destroy(s_window);
    s_window = NULL;

    game_free();

    app_timer_cancel(s_next_frame_timer);
    s_next_frame_timer = NULL;
}

void main_window_push()
{
    if (!s_window)
    {
        s_window = window_create();
        window_set_click_config_provider(s_window, click_config_provider);
        window_set_window_handlers(s_window, (WindowHandlers){
                                                 .load = window_load,
                                                 .unload = window_unload,
                                                 .appear = window_appear,
                                                 .disappear = window_disappear,
                                             });
    }
    window_stack_push(s_window, true);
}
