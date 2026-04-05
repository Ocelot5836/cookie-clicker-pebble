#include "dither.h"
#include <inttypes.h>

#define USE_RED
#define USE_GREEN
#define USE_BLUE
// #define DEBUG_DITHER

#define MATRIX_BITS 3
#define MATRIX_MASK ((1 << MATRIX_BITS) - 1)
#define MATRIX_SIZE_LENGTH (1 << MATRIX_BITS)
#define MATRIX_NORMALIZE (MATRIX_SIZE_LENGTH * MATRIX_SIZE_LENGTH)
#define MATRIX_FACTOR (2 * MATRIX_NORMALIZE)
#define MAX_VALUE (MATRIX_NORMALIZE - 1)

#define clamp(type, result, value, min, max)                                                             \
    type result;                                                                                         \
    {                                                                                                    \
        type _this_is_a_unique_name_dont_use_it_plz_0_ = value;                                          \
        type _this_is_a_unique_name_dont_use_it_plz_1_ = min;                                            \
        type _this_is_a_unique_name_dont_use_it_plz_2_ = max;                                            \
        result = _this_is_a_unique_name_dont_use_it_plz_0_ < _this_is_a_unique_name_dont_use_it_plz_1_   \
                     ? _this_is_a_unique_name_dont_use_it_plz_1_                                         \
                 : _this_is_a_unique_name_dont_use_it_plz_0_ > _this_is_a_unique_name_dont_use_it_plz_2_ \
                     ? _this_is_a_unique_name_dont_use_it_plz_2_                                         \
                     : _this_is_a_unique_name_dont_use_it_plz_0_;                                        \
    }
// #define matrix_val(x) (MATRIX_NORMALIZE * (2 * (x) - 3))
#define matrix_val(x) (128 * (x) / MATRIX_NORMALIZE - 128 * MAX_VALUE / MATRIX_NORMALIZE / 2)

#if MATRIX_BITS == 1
static const int32_t BAYER_MATRIX[] = {
    matrix_val(0), matrix_val(2),
    matrix_val(3), matrix_val(1)};
#elif MATRIX_BITS == 2
static const int32_t BAYER_MATRIX[] = {
    matrix_val(0), matrix_val(8), matrix_val(2), matrix_val(10),
    matrix_val(12), matrix_val(4), matrix_val(14), matrix_val(6),
    matrix_val(3), matrix_val(11), matrix_val(1), matrix_val(9),
    matrix_val(15), matrix_val(7), matrix_val(13), matrix_val(5)};
#elif MATRIX_BITS == 3
static const int32_t BAYER_MATRIX[] = {
    matrix_val(0), matrix_val(32), matrix_val(8), matrix_val(40), matrix_val(2), matrix_val(34), matrix_val(10), matrix_val(42),
    matrix_val(48), matrix_val(16), matrix_val(56), matrix_val(24), matrix_val(50), matrix_val(18), matrix_val(58), matrix_val(26),
    matrix_val(12), matrix_val(44), matrix_val(4), matrix_val(36), matrix_val(14), matrix_val(46), matrix_val(6), matrix_val(38),
    matrix_val(60), matrix_val(28), matrix_val(52), matrix_val(20), matrix_val(62), matrix_val(30), matrix_val(54), matrix_val(22),
    matrix_val(3), matrix_val(35), matrix_val(11), matrix_val(43), matrix_val(1), matrix_val(33), matrix_val(9), matrix_val(41),
    matrix_val(51), matrix_val(19), matrix_val(59), matrix_val(27), matrix_val(49), matrix_val(17), matrix_val(57), matrix_val(25),
    matrix_val(15), matrix_val(47), matrix_val(7), matrix_val(39), matrix_val(13), matrix_val(45), matrix_val(5), matrix_val(37),
    matrix_val(63), matrix_val(31), matrix_val(55), matrix_val(23), matrix_val(61), matrix_val(29), matrix_val(53), matrix_val(21)};
#endif

uint8_t evaluate_color(uint32_t color, int16_t x, int16_t y)
{
    int32_t transform = (BAYER_MATRIX[(x & MATRIX_MASK) + ((y & MATRIX_MASK) * MATRIX_SIZE_LENGTH)]);

    clamp(int32_t, transformedR, (int32_t)((color >> 16) & 0xFF) + transform, 0x00, 0xFF);
    clamp(int32_t, transformedG, (int32_t)((color >> 8) & 0xFF) + transform, 0x00, 0xFF);
    clamp(int32_t, transformedB, (int32_t)(color & 0xFF) + transform, 0x00, 0xFF);

    uint8_t r = (uint8_t)transformedR >> 6;
    uint8_t g = (uint8_t)transformedG >> 6;
    uint8_t b = (uint8_t)transformedB >> 6;

#ifdef DEBUG_DITHER
    if (x == 0 && y == 0)
    {
        int index = ((x & MATRIX_MASK) + ((y & MATRIX_MASK) * MATRIX_SIZE_LENGTH));
        APP_LOG(APP_LOG_LEVEL_INFO, "Red 0x%" PRIX32, (int32_t)((color >> 16) & 0xFF));
        APP_LOG(APP_LOG_LEVEL_INFO, "Color 0x%" PRIX32, (int32_t)color);
        APP_LOG(APP_LOG_LEVEL_INFO, "Transform %" PRId32, transform);
        APP_LOG(APP_LOG_LEVEL_INFO, "Index: %d", index);
        APP_LOG(APP_LOG_LEVEL_INFO, "Matrix: %" PRId32, BAYER_MATRIX[index]);
        APP_LOG(APP_LOG_LEVEL_INFO, "R %" PRIu32 " G %" PRIu32 " B %" PRIu32, transformedR, transformedG, transformedB);
    }
#endif

    return 0b11000000
#ifdef USE_RED
           | ((r & 0b11) << 4)
#endif
#ifdef USE_GREEN
           | ((g & 0b11) << 2)
#endif
#ifdef USE_BLUE
           | (b & 0b11)
#endif
        ;
}