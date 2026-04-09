#include "game.h"
#include "../windows/main_window.h"
#include "../windows/shop_window.h"
#include "cookie/cookie.h"
#include "cookie/cookie_particle.h"
#include "storage/storage.h"
#include "storage/tempnumbers.h"
#include "engine/numberformat.h"
#include "engine/math.h"
#include <math.h>

#define SIZE_DEBUG 0
#define SCREENSHOT_MODE 0

static uint64_t s_game_time;
static uint8_t s_animation_time;
static BigInt_t *s_cookie_count;
static BigInt_t *s_cookie_cpt;
static BigInt_t *s_sub_cookies;
static uint8_t s_building_counts[NUM_BUILDINGS];

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
    format_cookie_number(s_cookie_count, MAX_TEXT_LENGTH, s_text);
    format_cps_number(s_cookie_cpt, MAX_SUBTEXT_LENGTH, s_subtext);
    main_window_update_text();
}

void game_init(Window *window)
{
#if SIZE_DEBUG
    size_t pre_init_size = heap_bytes_free();
#endif

    s_game_time = 0;
    s_animation_time = 0;

    s_cookie_count = calloc(COOKIE_COUNTER_WORDS, BigIntWordSize);
    storage_read_cookies(s_cookie_count);

#if SCREENSHOT_MODE
    s_building_counts[BUILDING_TYPE_CURSOR] = 68;
    s_building_counts[BUILDING_TYPE_GRANDMA] = 70;
    s_building_counts[BUILDING_TYPE_FARM] = 43;
    s_building_counts[BUILDING_TYPE_MINE] = 28;
    s_building_counts[BUILDING_TYPE_FACTORY] = 23;
    s_building_counts[BUILDING_TYPE_BANK] = 17;
    s_building_counts[BUILDING_TYPE_TEMPLE] = 14;
    s_building_counts[BUILDING_TYPE_WIZARD_TOWER] = 6;
    s_building_counts[BUILDING_TYPE_SHIPMENT] = 1;
    s_building_counts[BUILDING_TYPE_ALCHEMY_LAB] = 0;
    s_building_counts[BUILDING_TYPE_PORTAL] = 0;
    s_building_counts[BUILDING_TYPE_TIME_MACHINE] = 0;
    s_building_counts[BUILDING_TYPE_ANTIMATTER_CONDENSER] = 0;
    s_building_counts[BUILDING_TYPE_PRISM] = 0;
    s_building_counts[BUILDING_TYPE_CHANCEMAKER] = 0;
    s_building_counts[BUILDING_TYPE_FRACTAL_ENGINE] = 0;
    s_building_counts[BUILDING_TYPE_JAVASCRIPT_CONSOLE] = 0;
    s_building_counts[BUILDING_TYPE_IDLEVERSE] = 0;
    s_building_counts[BUILDING_TYPE_CORTEX_BAKER] = 0;
    s_building_counts[BUILDING_TYPE_YOU] = 0;
#else
    storage_read_buildings(s_building_counts);
#endif
    buildings_init(s_building_counts);

    s_cookie_cpt = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    s_sub_cookies = calloc(COOKIE_COUNTER_WORDS, BigIntWordSize);
    building_get_cpt(s_building_counts, s_cookie_cpt);

    cookie_particles_kill();

    time_t epoch_time;
    storage_read_time(&epoch_time);

    // This adds the cookies collected in the background
    if (epoch_time != 0L)
    {
        storage_remove_time();

        BigInt_t *seconds_passed = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
        BigInt_t *tmp1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
        BigInt_t *tmp2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);

        time_t delta = time(NULL) - epoch_time;
        BigInt_from_int(COOKIE_COUNTER_WORDS, seconds_passed, delta);
        BigInt_mul_basic(COOKIE_COUNTER_WORDS, s_cookie_cpt, seconds_passed, tmp1);
        BigInt_from_int(COOKIE_COUNTER_WORDS, tmp2, TPS);
        BigInt_div(COOKIE_COUNTER_WORDS, tmp1, tmp2, seconds_passed);
        BigInt_add(COOKIE_COUNTER_WORDS, seconds_passed, COOKIE_COUNTER_WORDS, s_cookie_count, COOKIE_COUNTER_WORDS, tmp1);
        BigInt_copy(COOKIE_COUNTER_WORDS, s_cookie_count, tmp1);

        free(seconds_passed);
        free(tmp1);
        free(tmp2);

        storage_write_cookies(s_cookie_count);
        update_text();
        shop_on_cookie_change();
    }

#if SIZE_DEBUG
    APP_LOG(APP_LOG_LEVEL_INFO, "Used %zu bytes during init", pre_init_size - heap_bytes_free());
#endif
}

void game_free()
{
    free(s_cookie_count);
    free(s_cookie_cpt);
    free(s_sub_cookies);
    buildings_free();

    time_t epoch_time = time(NULL);
    storage_write_time(&epoch_time);
}

void game_init_resources()
{
    cookie_load(RESOURCE_ID_COOKIE);
    update_text();
}

void game_free_resources()
{
    cookie_free();
}

bool game_update(Window *window, GRect bounds)
{
    s_game_time++;
    if (s_animation_time > 0)
    {
        s_animation_time--;
    }

#if TIME_LOGGING
    uint16_t startMs = time_ms(NULL, NULL);
#endif
    if (!BigInt_is_zero(COOKIE_COUNTER_WORDS, s_cookie_cpt))
    {
        BigInt_add(COOKIE_COUNTER_WORDS, s_cookie_cpt, COOKIE_COUNTER_WORDS, s_sub_cookies, COOKIE_COUNTER_WORDS, number_temp1);
        BigInt_copy(COOKIE_COUNTER_WORDS, s_sub_cookies, number_temp1);
        BigInt_from_int(COOKIE_COUNTER_WORDS, number_temp1, TPS * TPS);

        if (BigInt_cmp(COOKIE_COUNTER_WORDS, s_sub_cookies, number_temp1) != SMALLER)
        {
            BigInt_div(COOKIE_COUNTER_WORDS, s_sub_cookies, number_temp1, number_temp2);

            uint8_t particle_count = (uint8_t)(BigInt_to_int(COOKIE_COUNTER_WORDS, number_temp2) & 0xF);

            BigInt_mul(COOKIE_COUNTER_WORDS, number_temp2, COOKIE_COUNTER_WORDS, number_temp1, COOKIE_COUNTER_WORDS, number_temp3);
            BigInt_sub(COOKIE_COUNTER_WORDS, s_sub_cookies, COOKIE_COUNTER_WORDS, number_temp3, COOKIE_COUNTER_WORDS, number_temp1);
            BigInt_copy(COOKIE_COUNTER_WORDS, s_sub_cookies, number_temp1);

            BigInt_add(COOKIE_COUNTER_WORDS, s_cookie_count, COOKIE_COUNTER_WORDS, number_temp2, COOKIE_COUNTER_WORDS, number_temp1);
            BigInt_copy(COOKIE_COUNTER_WORDS, s_cookie_count, number_temp1);

            for (size_t i = 0; i < particle_count; i++)
            {
                cookie_particle_spawn();
            }

            storage_write_cookies(s_cookie_count);
            update_text();
            shop_on_cookie_change();
        }
    }

    cookie_particles_tick(&bounds);

    return true;
}

void game_draw_first(Layer *layer, GContext *ctx)
{
    GRect bounds = layer_get_bounds(layer);

    graphics_context_set_compositing_mode(ctx, GCompOpSet);

    if (cookie_particle_status != 0)
    {
        for (size_t i = __builtin_ctz(cookie_particle_status); i < MAX_COOKIE_PARTICLES; i++)
        {
            if (((cookie_particle_status >> i) & 1) == 0)
            {
                continue;
            }

            int16_t x = cookie_particle_x[i];
            int16_t y = cookie_particle_y[i] - COOKIE_PARTICLE_YOFF;

            // Don't draw cookies behind the big cookie
            int16_t dx = x - bounds.size.w / 2;
            int16_t dy = y - bounds.size.h / 2;
            if (dx * dx + dy * dy < cookie_size * cookie_size / 4 - 4 * COOKIE_PARTICLE_WIDTH * COOKIE_PARTICLE_WIDTH)
            {
                continue;
            }

            graphics_draw_bitmap_in_rect(ctx, s_cookie_effect, GRect(cookie_particle_x[i], cookie_particle_y[i] - COOKIE_PARTICLE_YOFF, COOKIE_PARTICLE_WIDTH, COOKIE_PARTICLE_HEIGHT));
        }
    }
}

void game_draw(Layer *layer, GContext *ctx, GBitmap *fb)
{
    GRect bounds = layer_get_bounds(layer);

    int angle = s_game_time * 10 / TPS;
    cookie_draw(layer, fb, &GPoint(bounds.size.w / 2, bounds.size.h / 2), DEG_TO_TRIGANGLE(angle), get_scale(GAME_MAX_ANIMATION_TIME - s_animation_time));
}

void game_click()
{
    s_animation_time = GAME_MAX_ANIMATION_TIME;
    BigInt_inc(COOKIE_COUNTER_WORDS, s_cookie_count);
    storage_write_cookies(s_cookie_count);
    update_text();
    shop_on_cookie_change();
    cookie_particle_spawn();
}

uint8_t game_purchase(BuildingType building, uint8_t count)
{
    uint8_t to_buy_count = MIN(255 - s_building_counts[building], count);
    if (to_buy_count == 0)
    {
        return 0;
    }

    BigInt_t *tmp = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    int8_t i;

    for (i = 0; i < to_buy_count; i++)
    {
        BigInt_t *building_cost = building_get_cost(building);
        if (BigInt_cmp(COOKIE_COUNTER_WORDS, s_cookie_count, building_cost) == SMALLER)
        {
            break;
        }

        BigInt_sub(COOKIE_COUNTER_WORDS, s_cookie_count, COOKIE_COUNTER_WORDS, building_cost, COOKIE_COUNTER_WORDS, tmp);
        BigInt_copy(COOKIE_COUNTER_WORDS, s_cookie_count, tmp);

        s_building_counts[building]++;
        building_update_cost(building, s_building_counts[building]);
    }

    free(tmp);

    storage_write_cookies(s_cookie_count);
    storage_write_buildings(s_building_counts);
    building_get_cpt(s_building_counts, s_cookie_cpt);
    update_text();
    shop_on_cookie_change();

    return i;
}

uint8_t *game_get_building_counts()
{
    return s_building_counts;
}

BigInt_t *game_get_cookie_count()
{
    return s_cookie_count;
}
