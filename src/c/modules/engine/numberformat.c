#include "numberformat.h"
#include "../storage/storage.h"
#include "../../windows/main_window.h"
#include <math.h>

#define NUMBER_NAMES_COUNT 21
#define PRECISION 3

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

BigInt_t *cmp;
BigInt_t *divide;
BigInt_t *store1;
BigInt_t *store2;

static int format_number_manual(char *buf, size_t size, double val, const char *suffix, const uint8_t precision, bool add_cookies)
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

    if (frac_part == 0)
    {
        return snprintf(buf, size, add_cookies ? "%lld\n%s cookies" : "%lld %s", int_part, suffix);
    }
    else
    {
        return snprintf(buf, size, add_cookies ? "%lld.%0*lld\n%s cookies" : "%lld.%0*lld %s", int_part, precision, frac_part, suffix);
    }
}

void format_init()
{
    cmp = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    divide = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_from_int(COOKIE_COUNTER_WORDS, divide, 1000);
    store1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    store2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
}

void format_free()
{
    free(cmp);
    cmp = NULL;
    free(divide);
    divide = NULL;
    free(store1);
    store1 = NULL;
    free(store2);
    store2 = NULL;
}

void format_cookie_number(BigInt_t *value, size_t max_length, char *buffer)
{
    memset(buffer, 0, sizeof(char) * (max_length + 1));

    BigInt_from_int(COOKIE_COUNTER_WORDS, cmp, 1000000);
    if (BigInt_cmp(COOKIE_COUNTER_WORDS, value, cmp) != SMALLER)
    {
        BigInt_copy(COOKIE_COUNTER_WORDS, store1, value);

        for (size_t i = 0; i <= NUMBER_NAMES_COUNT; i++)
        {
            BigInt_t *from = (i & 1) ? store2 : store1;
            BigInt_t *to = (i & 1) ? store1 : store2;

            if (i == NUMBER_NAMES_COUNT || BigInt_cmp(COOKIE_COUNTER_WORDS, from, cmp) == SMALLER)
            {
                format_number_manual(buffer, max_length, (double)BigInt_to_long(COOKIE_COUNTER_WORDS, to) / 1000000.0, s_names[i - 1], 3, true);
                break;
            }
            BigInt_div(COOKIE_COUNTER_WORDS, from, divide, to);
        }
    }
    else
    {
        snprintf(buffer, max_length, "%llu\ncookies", BigInt_to_long(COOKIE_COUNTER_WORDS, value));
    }
}

void format_cps_number(BigInt_t *cpt, size_t max_length, char *buffer)
{
    memset(buffer, 0, sizeof(char) * (max_length + 1));

    double cps = (double)BigInt_to_long(COOKIE_COUNTER_WORDS, cpt) / (double)TPS;
    uint64_t int_part = (uint64_t)cps;

    double diff = cps - (double)int_part;
    uint64_t frac_part = (uint64_t)(diff * 10 + 0.5); // +0.5 for rounding

    if (frac_part == 0)
    {
        snprintf(buffer, max_length, "per second: %llu", int_part);
    }
    else
    {
        snprintf(buffer, max_length, "per second: %llu.%lld", int_part, frac_part);
    }
}

uint32_t format_number(BigInt_t *value, size_t max_length, char *buffer)
{
    memset(buffer, 0, max_length);
    uint32_t written = 0;

    BigInt_from_int(COOKIE_COUNTER_WORDS, cmp, 1000000);
    if (BigInt_cmp(COOKIE_COUNTER_WORDS, value, cmp) != SMALLER)
    {
        BigInt_copy(COOKIE_COUNTER_WORDS, store1, value);
        BigInt_from_int(COOKIE_COUNTER_WORDS, cmp, 1000);

        for (size_t i = 0; i < NUMBER_NAMES_COUNT; i++)
        {
            BigInt_t *from = (i & 1) ? store2 : store1;
            BigInt_t *to = (i & 1) ? store1 : store2;

            BigInt_div(COOKIE_COUNTER_WORDS, from, cmp, to);
            if (i == NUMBER_NAMES_COUNT - 1 || BigInt_cmp(COOKIE_COUNTER_WORDS, from, cmp) == SMALLER)
            {
                BigInt_mul(COOKIE_COUNTER_WORDS, from, 1, cmp, COOKIE_COUNTER_WORDS, to);
                written = format_number_manual(buffer, max_length, (double)BigInt_to_long(COOKIE_COUNTER_WORDS, to) / 1000.0, s_names[i], 1, false);
                break;
            }
        }
    }
    else
    {
        written = snprintf(buffer, max_length, "%llu", BigInt_to_long(COOKIE_COUNTER_WORDS, value));
    }

    return written;
}
