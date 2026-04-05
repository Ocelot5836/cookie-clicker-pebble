#include "cookie.h"
#include "../engine/pixel.h"
#include "../engine/math.h"

#define PBL_COLOR 1
#define PADDING 20

static GBitmap *s_cookie;
static uint8_t s_cookie_size;

typedef struct DivResult
{
    int16_t quot;
    int16_t rem;
} DivResult;

//! a div and mod operation where any remainder will always be the same direction as the numerator
static DivResult polar_div(int32_t numer, uint32_t denom)
{
    DivResult res;
    res.quot = numer / denom;
    res.rem = numer % denom;
    if (numer < 0 && res.rem > 0)
    {
        res.rem -= denom;
        res.quot += denom;
    }
    return res;
}

#ifdef PBL_BW
static bool get_bitmap_bit(GBitmapDataRowInfo info, uint16_t x)
{
    // Read the single bit of the correct byte
    uint8_t byte = x >> 3;
    uint8_t bit = x & 7;
    return ((info.data[x >> 3]) >> (x & 7)) & 1;
}
#endif

void cookie_load(uint32_t resource_id)
{
    s_cookie = gbitmap_create_with_resource(resource_id);

    GRect bounds = gbitmap_get_bounds(s_cookie);
    s_cookie_size = MIN(bounds.size.w, bounds.size.h);
}

void cookie_free()
{
    gbitmap_destroy(s_cookie);
}

void cookie_draw(Layer *layer, GBitmap *fb, GPoint *pos, int32_t rotation, int32_t scale)
{
    GRect dest_clip = layer_get_bounds(layer);
    GPoint src_ic = GPoint(s_cookie_size >> 1, s_cookie_size >> 1);
    GPoint dest_ic = GPoint(pos->x, pos->y);

    grect_clip(&dest_clip, &GRect(dest_ic.x - src_ic.x - PADDING, dest_ic.y - src_ic.y - PADDING, s_cookie_size + PADDING * 2, s_cookie_size + PADDING * 2));

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
            const int32_t src_numerator_x = (cos_value * dx - sin_value * dy) - scale * dx;
            const int32_t src_numerator_y = (cos_value * dy + sin_value * dx) - scale * dy;

            const DivResult src_vector_x = polar_div(src_numerator_x, TRIG_MAX_RATIO);
            const DivResult src_vector_y = polar_div(src_numerator_y, TRIG_MAX_RATIO);

            const int16_t src_x = src_ic.x + src_vector_x.quot;
            const int16_t src_y = src_ic.y + src_vector_y.quot;

            // only draw if within the src range
            const GBitmapDataRowInfo src_info = gbitmap_get_data_row_info(s_cookie, src_y);
            if (!(WITHIN(src_x, 0, s_cookie_size - 1) &&
                  WITHIN(src_y, 0, s_cookie_size - 1) &&
                  WITHIN(src_x, src_info.min_x, src_info.max_x)))
            {
                continue;
            }

#if PBL_BW
            // dividing by 8 to avoid overflows of <thresh> in the next loop
            const int32_t horiz_contrib[3] = {
                src_vector_x.rem < 0 ? (-src_vector_x.rem) >> 3 : 0,
                src_vector_x.rem < 0 ? (TRIG_MAX_RATIO + src_vector_x.rem) >> 3 : (TRIG_MAX_RATIO - src_vector_x.rem) >> 3,
                src_vector_x.rem < 0 ? 0 : (src_vector_x.rem) >> 3};

            const int32_t vert_contrib[3] = {
                src_vector_y.rem < 0 ? (-src_vector_y.rem) >> 3 : 0,
                src_vector_y.rem < 0 ? (TRIG_MAX_RATIO + src_vector_y.rem) >> 3 : (TRIG_MAX_RATIO - src_vector_y.rem) >> 3,
                src_vector_y.rem < 0 ? 0 : (src_vector_y.rem) >> 3};

            int32_t thresh = 0;

            int8_t startX = src_x == 0 ? 0 : -1;
            int8_t startY = src_y == 0 ? 0 : -1;
            int8_t endX = src_x == dest_clip.size.w - 1 ? 0 : 1;
            int8_t endY = src_y == dest_clip.size.h - 1 ? 0 : 1;
            for (int yy = startY; yy <= endY; yy++)
            {
                const GBitmapDataRowInfo row_info = gbitmap_get_data_row_info(s_cookie, src_y + yy);
                for (int xx = -1; xx <= 1; xx++)
                {
                    if (src_x + xx >= 0 && src_x + xx < dest_clip.size.w && src_y + yy >= 0 && src_y + yy < dest_clip.size.h)
                    {
                        // I'm within bounds
                        if (((row_info.data[(src_x + xx) >> 3]) >> ((src_x + xx) & 7)) & 1)
                        {
                            // more color
                            thresh += (horiz_contrib[xx + 1] * vert_contrib[yy + 1]);
                        }
                        else
                        {
                            // less color
                            thresh -= (horiz_contrib[xx + 1] * vert_contrib[yy + 1]);
                        }
                    }
                }
            }

            if (thresh > 0)
            {
                set_pixel_color(dest_info, x, GColorBlack);
            }
#elif PBL_COLOR
            if (src_info.data[src_x] != GColorClearARGB8)
            {
                memset(&dest_info.data[x], src_info.data[src_x], 1);
            }
#endif
        }
    }
}
