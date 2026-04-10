#pragma once

#include <pebble.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define ABS(a) (((a) > 0) ? (a) : -1 * (a))
#define CLIP(n, min, max) ((n)<(min)?(min):((n)>(max)?(max):(n)))
#define ROUND(num, denom) (((num) + ((denom) / 2))/(denom))
#define WITHIN(n, min, max) ((n) >= (min) && (n) <= (max))
#define RANGE_WITHIN(n_min, n_max, min, max) ((n_min) >= (min) && (n_max) <= (max))
