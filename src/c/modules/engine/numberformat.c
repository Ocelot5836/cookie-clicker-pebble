#include "numberformat.h"
#include "../storage/storage.h"
#include "../storage/tempnumbers.h"
#include "../../windows/main_window.h"
#include <math.h>

#define NUMBER_NAMES_COUNT 21
#define PRECISION 3

const char *const s_names[] = {
    "m",
    "b",
    "tr",
    "quadr",
    "quint",
    "sext",
    "sept",
    "oct",
    "non",
    "dec",
    "undec",
    "duodece",
    "redec",
    "quattuordec",
    "quindec",
    "sexdec",
    "septendec",
    "octodec",
    "novemdec",
    "vigint",
    "cent",
};

static int format_number_manual(char *buf, size_t size, double val, const char *suffix, const uint8_t precision, bool split)
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
        return snprintf(buf, size, split ? "%lld\n%s%s" : "%lld %s%s", int_part, suffix, "illion");
    }
    else
    {
        return snprintf(buf, size, split ? "%lld.%0*lld\n%s%s" : "%lld.%0*lld %s%s", int_part, precision, frac_part, suffix, "illion");
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

uint32_t format_number(BigInt_t *value, size_t max_length, char *buffer, bool split)
{
    memset(buffer, 0, max_length);
    uint32_t written = 0;

    BigInt_from_int(COOKIE_COUNTER_WORDS, number_temp1, 1000000);
    BigInt_from_int(COOKIE_COUNTER_WORDS, number_temp2, 1000);

    if (BigInt_cmp(COOKIE_COUNTER_WORDS, value, number_temp1) != SMALLER)
    {
        BigInt_copy(COOKIE_COUNTER_WORDS, number_temp3, value);

        for (size_t i = 0; i <= NUMBER_NAMES_COUNT; i++)
        {
            BigInt_t *from;
            BigInt_t *to;

            if ((i & 1))
            {
                from = number_temp4;
                to = number_temp3;
            }
            else
            {
                from = number_temp3;
                to = number_temp4;
            }

            if (i == NUMBER_NAMES_COUNT || BigInt_cmp(COOKIE_COUNTER_WORDS, from, number_temp1) == SMALLER)
            {
                written = format_number_manual(buffer, max_length, (double)BigInt_to_long(COOKIE_COUNTER_WORDS, to) / 1000000.0, s_names[i - 1], 3, split);
                break;
            }
            BigInt_div(COOKIE_COUNTER_WORDS, from, number_temp2, to);
        }
    }
    else
    {
        written = snprintf(buffer, max_length, split ? "%llu\n" : "%llu", BigInt_to_long(COOKIE_COUNTER_WORDS, value));
    }

    return written;
}
