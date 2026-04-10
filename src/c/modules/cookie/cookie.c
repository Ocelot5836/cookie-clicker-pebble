#include "../flags.h"
#include "cookie.h"
#include "../engine/math.h"

#define PADDING 20

GBitmap *s_cookie;
#if PARTICLE_ENABLE
GBitmap *s_cookie_effect;
#endif
uint8_t cookie_size;

//! a div and mod operation where any remainder will always be the same direction as the numerator
static int16_t polar_div(int32_t numer, uint32_t denom)
{
    int16_t res = numer / denom;
    if (numer < 0 && (numer % denom) > 0)
    {
        res += denom;
    }
    return res;
}

void cookie_load(uint32_t resource_id)
{
    s_cookie = gbitmap_create_with_resource(resource_id);
#if PARTICLE_ENABLE
    s_cookie_effect = gbitmap_create_with_resource(RESOURCE_ID_COOKIE_EFFECT);
#endif

    GRect bounds = gbitmap_get_bounds(s_cookie);
    cookie_size = MIN(bounds.size.w, bounds.size.h);
}

void cookie_free()
{
    gbitmap_destroy(s_cookie);
#if PARTICLE_ENABLE
    gbitmap_destroy(s_cookie_effect);
#endif
}

// Based on https://github.com/coredevices/PebbleOS/blob/ef24b82802ba608eba87b9d6c5d6a9d1562ea397/src/fw/applib/graphics/graphics_bitmap.c#L131
void cookie_draw(Layer *layer, GBitmap *fb, GPoint *pos, int32_t rotation, uint8_t scale)
{
    GRect dest_clip = layer_get_bounds(layer);
    GPoint src_ic = GPoint(cookie_size / 2, cookie_size / 2);
    GPoint dest_ic = GPoint(pos->x, pos->y);
    int32_t base_scale = COOKIE_SCALE_INCREMENT * (MIN(dest_clip.size.w, dest_clip.size.h) - cookie_size - PBL_IF_ROUND_ELSE(60, 30)) / PBL_IF_ROUND_ELSE(12, 6);

    grect_clip(&dest_clip, &GRect(dest_ic.x - src_ic.x - PADDING, dest_ic.y - src_ic.y - PADDING, cookie_size + 2 * PADDING, cookie_size + 2 * PADDING));

    const int32_t cos_value = cos_lookup(-rotation);
    const int32_t sin_value = sin_lookup(-rotation);

    uint16_t startY = dest_clip.origin.y;
    uint16_t endY = dest_clip.origin.y + dest_clip.size.h;
    for (int y = startY; y < endY; y++)
    {
        const GBitmapDataRowInfo dest_info = gbitmap_get_data_row_info(fb, y);
        int startX = MAX(dest_clip.origin.x, dest_info.min_x);
        int endX = MIN(dest_clip.origin.x + dest_clip.size.w, dest_info.max_x);

        const int32_t dy = y - dest_ic.y;
        for (int x = startX; x < endX; x++)
        {
            const int32_t dx = x - dest_ic.x;
            const int32_t src_numerator_x = (cos_value * dx - sin_value * dy) * (scale - base_scale + MAX_COOKIE_SCALE) / MAX_COOKIE_SCALE;
            const int32_t src_numerator_y = (cos_value * dy + sin_value * dx) * (scale - base_scale + MAX_COOKIE_SCALE) / MAX_COOKIE_SCALE;

            int16_t src_vector_x = polar_div(src_numerator_x, TRIG_MAX_RATIO);
            int16_t src_vector_y = polar_div(src_numerator_y, TRIG_MAX_RATIO);

            // only draw if within the src range
            if (src_vector_x * src_vector_x + src_vector_y * src_vector_y >= cookie_size * cookie_size / 4)
            {
                continue;
            }

            const int16_t src_x = src_ic.x + src_vector_x;
            const int16_t src_y = src_ic.y + src_vector_y;
            const GBitmapDataRowInfo src_info = gbitmap_get_data_row_info(s_cookie, src_y);

#if PBL_BW
#if !DEBUG_NO_CLIP
            if (((src_info.data[src_x >> 3]) >> (src_x & 7)) & 1)
#endif
            {
                uint8_t byte = x >> 3;
                uint8_t bit = x & 7;
                dest_info.data[byte] &= ~(1 << bit);
                dest_info.data[byte] = 0;
            }
#elif PBL_COLOR
#if !DEBUG_NO_CLIP
            if (src_info.data[src_x] != GColorClearARGB8)
#endif
            {
                memset(&dest_info.data[x], src_info.data[src_x], 1);
            }
#endif
        }
    }
}
