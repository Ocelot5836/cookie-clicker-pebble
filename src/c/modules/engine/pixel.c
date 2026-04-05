#include "pixel.h"

// void byte_set_bit(uint8_t *byte, uint8_t bit, uint8_t value)
// {
//     *byte ^= (-value ^ *byte) & (1 << bit);
// }

GColor get_pixel_color(GBitmapDataRowInfo info, uint16_t x)
{
#if defined(PBL_COLOR)
    // Read the single byte color pixel
    return (GColor){.argb = info.data[x]};
#elif defined(PBL_BW)
    // Read the single bit of the correct byte
    uint8_t byte = x >> 3;
    uint8_t bit = x & 7;
    return (info.data[byte] >> bit) & 1 ? GColorWhite : GColorBlack;
#endif
}

void set_pixel_color(GBitmapDataRowInfo info, uint16_t x, GColor color)
{
#if defined(PBL_COLOR)
    // Write the pixel's byte color
    memset(&info.data[x], color.argb, 1);
#elif defined(PBL_BW)
    // Find the correct byte, then set the appropriate bit
    uint8_t byte = x >> 3;
    uint8_t bit = x & 7;
    // byte_set_bit(&info.data[byte], bit, gcolor_equal(color, GColorWhite) ? 1 : 0);
    info.data[byte] ^= (-(gcolor_equal(color, GColorWhite) ? 1 : 0) ^ info.data[byte]) & (1 << bit);
#endif
}