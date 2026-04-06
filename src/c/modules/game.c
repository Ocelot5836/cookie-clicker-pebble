#include "game.h"
#include "cookie/cookie.h"
#include "storage/storage.h"
#include <math.h>

#define MAX_TEXT_LENGTH 50

static uint64_t s_game_time;
static uint8_t s_animation_time;
static BigInt_t *s_cookie_count;
static char s_counter[MAX_TEXT_LENGTH + 1];

#define NUMBER_NAMES_COUNT 21

const char *const s_names[] = {
    "million",
    "billion",
    "trillion",
    "quadrillion",
    "quintillion",
    "sextillion",
    "septillion",
    "octillion",
    "nonillion",
    "decillion",
    "undecillion",
    "duodeceillion",
    "redecillion",
    "quattuordecillion",
    "quindecillion",
    "sexdecillion",
    "septendecillion",
    "octodecillion",
    "novemdecillion",
    "vigintillion",
    "centillion",
};

static uint8_t get_scale(uint8_t x)
{
    return x == 0
               ? 0
           : x == 1
               ? COOKIE_SCALE_INCREMENT
               : (pow(2.0, -10.0 * x / GAME_MAX_ANIMATION_TIME) * (double)sin_lookup(DEG_TO_TRIGANGLE(x * 1200 / GAME_MAX_ANIMATION_TIME - 90)) / TRIG_MAX_ANGLE + 1.0) * COOKIE_SCALE_INCREMENT;
}

static int manual_snprintf_float(char *buf, size_t size, double val, int precision, const char *suffix)
{
    if (val < 0)
    {
        *buf++ = '-';
        size--;
        val = -val;
    }

    uint64_t int_part = (uint64_t)val;

    double diff = val - (double)int_part;
    uint64_t frac_part = (uint64_t)(diff * pow(10, precision) + 0.5); // +0.5 for rounding

    return snprintf(buf, size, "%lld.%0*lld\n%s cookies", int_part, precision, frac_part, suffix);
}

static void update_text()
{
    uint8_t written = 0;

    BigInt_t *cmp = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *store1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *store2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);

    BigInt_from_int(COOKIE_COUNTER_WORDS, cmp, 1000000);
    if (BigInt_cmp(COOKIE_COUNTER_WORDS, s_cookie_count, cmp) != SMALLER)
    {
        BigInt_copy(COOKIE_COUNTER_WORDS, store1, s_cookie_count);
        BigInt_from_int(COOKIE_COUNTER_WORDS, cmp, 1000);

        for (size_t i = 0; i < NUMBER_NAMES_COUNT; i++)
        {
            BigInt_t *from = (i & 1) ? store2 : store1;
            BigInt_t *to = (i & 1) ? store1 : store2;

            BigInt_div(COOKIE_COUNTER_WORDS, from, cmp, to);
            if (i == NUMBER_NAMES_COUNT - 1 || BigInt_cmp(COOKIE_COUNTER_WORDS, from, cmp) == SMALLER)
            {
                BigInt_mul(COOKIE_COUNTER_WORDS, from, 1, cmp, COOKIE_COUNTER_WORDS, to);
                written = manual_snprintf_float(s_counter, MAX_TEXT_LENGTH, (double)BigInt_to_long(COOKIE_COUNTER_WORDS, to) / 1000.0, 3, s_names[i]);
                break;
            }
        }
    }
    else
    {
        written = snprintf(s_counter, MAX_TEXT_LENGTH, "%llu\ncookies", BigInt_to_long(COOKIE_COUNTER_WORDS, s_cookie_count));
    }

    free(cmp);
    free(store1);
    free(store2);

    s_counter[written] = 0;
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