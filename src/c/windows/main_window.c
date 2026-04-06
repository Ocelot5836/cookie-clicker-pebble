#include "main_window.h"
#include "shop_window.h"
#include "../modules/game.h"

static Window *s_window;
static Layer *s_render_layer;
static GFont s_font;
static TextLayer *s_text_layer;
static AppTimer *s_next_frame_timer;
static bool s_screen_dirty;

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
    game_click();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{
    shop_window_push();
}

static void click_config_provider(void *context)
{
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void next_frame_handler(void *context)
{
    if (s_next_frame_timer == NULL)
    {
        return;
    }

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

#if TIME_LOGGING
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

#if TIME_LOGGING
    uint16_t startDrawMs = time_ms(NULL, NULL);
#endif
    game_draw(layer, ctx, fb);
#if TIME_LOGGING
    uint16_t endDrawMs = time_ms(NULL, NULL);
#endif

    graphics_release_frame_buffer(ctx, fb);

#if TIME_LOGGING
    uint16_t startDrawLastMs = time_ms(NULL, NULL);
#endif
    game_draw_last(layer, ctx);
#ifdef TIME_LOGGING
    uint16_t endDrawLastMs = time_ms(NULL, NULL);
#endif

#if TIME_LOGGING
    uint16_t delta = (endDrawMs - startDrawMs) + (endDrawLastMs - startDrawLastMs);
    if (delta > 0)
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "Took %dms (%dms, %dms) to draw", delta, endDrawMs - startDrawMs, endDrawLastMs - startDrawLastMs);
    }
#endif
}

static void window_appear(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "APPEAR");

    game_init_resources();

    s_screen_dirty = true;
    s_next_frame_timer = app_timer_register(DELTA, next_frame_handler, NULL);
}

static void window_disappear(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "DISAPPEAR");

    game_free_resources();

    s_screen_dirty = false;
    app_timer_cancel(s_next_frame_timer);
    s_next_frame_timer = NULL;
}

static void window_load(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "LOAD");
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COOKIE_FONT_20));

    s_render_layer = layer_create(bounds);
    layer_set_update_proc(s_render_layer, render_handler);
    layer_add_child(window_layer, s_render_layer);

    s_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(GRect(bounds.origin.x, bounds.origin.y + 20, bounds.size.w, bounds.size.h), bounds));
    text_layer_set_font(s_text_layer, s_font);
    text_layer_set_background_color(s_text_layer, GColorClear);
    text_layer_set_text_color(s_text_layer, GColorWhite);
    text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

    s_next_frame_timer = app_timer_register(DELTA, next_frame_handler, NULL);

    game_init(window);
}

static void window_unload(Window *window)
{
    APP_LOG(APP_LOG_LEVEL_INFO, "UNLOAD");

    layer_destroy(s_render_layer);
    s_render_layer = NULL;

    fonts_unload_custom_font(s_font);
    s_font = NULL;

    text_layer_destroy(s_text_layer);
    s_text_layer = NULL;

    game_free();
}

void main_window_push()
{
    if (!s_window)
    {
        s_window = window_create();
        window_set_background_color(s_window, PBL_IF_BW_ELSE(GColorBlack, GColorCobaltBlue));
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

void main_window_free()
{
    window_destroy(s_window);
}

void main_window_set_text(char *text)
{
    text_layer_set_text(s_text_layer, text);
    layer_mark_dirty(text_layer_get_layer(s_text_layer));
    s_screen_dirty = true;
}
