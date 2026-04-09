#include "shop_window.h"
#include "../modules/engine/numberformat.h"
#include "../modules/engine/math.h"
#include "../modules/engine/gbitmap_color_palette_manipulator.h"
#include "../modules/buildings/buildings.h"
#include "../modules/storage/storage.h"
#include "../modules/game.h"
#include "main_window.h"

#define MAX_SHOP_ITEM_TITLE_LENGTH 30
#define MAX_SHOP_ITEM_SUBTITLE_LENGTH 50
#define SHOP_TEXT_AREA_HEIGHT 32
#define SHOP_TEXT_AREA_Y PBL_IF_ROUND_ELSE(8, 0)

const char *const s_building_names[] = {
    "Cursor",
    "Grandma",
    "Farm",
    "Mine",
    "Factory",
    "Bank",
    "Temple",
    "Wizard Tower",
    "Shipment",
    "Alchemy Lab",
    "Portal",
    "Time Machine",
    "Antimatter Condenser",
    "Prism",
    "Chancemaker",
    "Fractal Engine",
    "Javascript Console",
    "Idleverse",
    "Cortex Baker",
    "You",
};

static Window *s_window;
static GFont s_font;
static TextLayer *s_cps_layer;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[1];
static SimpleMenuItem s_first_menu_items[NUM_BUILDINGS];

static char *s_option_titles[NUM_BUILDINGS];
static char *s_option_subtitles[NUM_BUILDINGS];

static void update_menu()
{
    uint8_t *building_counts = game_get_building_counts();
    int8_t last_purchased_index = -1;

    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        if (building_counts[i] != 0)
        {
            last_purchased_index = i;
        }
    }

    uint32_t count = MIN((uint32_t)(last_purchased_index + 3), NUM_BUILDINGS);
    if (s_menu_sections[0].num_items == count)
    {
        if (last_purchased_index < 0)
        {
            return;
        }

        for (size_t i = 0; i <= (uint8_t)last_purchased_index; i++)
        {
            BigInt_t *building_cost = building_get_cost((BuildingType)i);
            uint32_t offset = format_number(building_cost, MAX_SHOP_ITEM_SUBTITLE_LENGTH, s_option_subtitles[i]);
            snprintf(s_option_subtitles[i] + offset, MAX_SHOP_ITEM_SUBTITLE_LENGTH - offset, " cookies");
            snprintf(s_option_titles[i], MAX_SHOP_ITEM_TITLE_LENGTH, "x%d %s", building_counts[i], s_building_names[i]);
        }
        return;
    }

    s_menu_sections[0].num_items = count;
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        bool hidden = last_purchased_index < 0 || i > (uint8_t)last_purchased_index;

        if (i < count)
        {
            if (s_first_menu_items[i].icon != NULL)
            {
                gbitmap_destroy(s_first_menu_items[i].icon);
            }
            s_first_menu_items[i].icon = gbitmap_create_with_resource(RESOURCE_ID_BUILDING_TYPE_CURSOR + i);
        }
        else if (s_first_menu_items[i].icon != NULL)
        {
            gbitmap_destroy(s_first_menu_items[i].icon);
            s_first_menu_items[i].icon = NULL;
        }

        BigInt_t *building_cost = building_get_cost((BuildingType)i);

        uint32_t offset = format_number(building_cost, MAX_SHOP_ITEM_SUBTITLE_LENGTH, s_option_subtitles[i]);
        snprintf(s_option_subtitles[i] + offset, MAX_SHOP_ITEM_SUBTITLE_LENGTH - offset, " cookies");

        if (hidden)
        {
            strncpy(s_option_titles[i], "???", 4);
#if PBL_COLOR
            gbitmap_fill_all_except(GColorOxfordBlue, GColorOxfordBlue, false, s_first_menu_items[i].icon, NULL);
#endif
        }
        else
        {
            snprintf(s_option_titles[i], MAX_SHOP_ITEM_TITLE_LENGTH, "x%d %s", building_counts[i], s_building_names[i]);
        }
    }
}

static void menu_select_callback(int index, void *ctx)
{
    if (game_purchase((BuildingType)index, 1) == 0)
    {
        return;
    }

    layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        s_option_titles[i] = calloc(MAX_SHOP_ITEM_TITLE_LENGTH + 1, sizeof(char));
        s_option_subtitles[i] = calloc(MAX_SHOP_ITEM_SUBTITLE_LENGTH + 1, sizeof(char));

        s_first_menu_items[i] = (SimpleMenuItem){
            .title = s_option_titles[i],
            .subtitle = s_option_subtitles[i],
            .callback = menu_select_callback,
            .icon = NULL,
        };
    }

    s_menu_sections[0] = (SimpleMenuSection){
        .items = s_first_menu_items,
        .num_items = 0,
    };

    update_menu();

    s_simple_menu_layer = simple_menu_layer_create(GRect(bounds.origin.x, bounds.origin.y + SHOP_TEXT_AREA_HEIGHT, bounds.size.w, bounds.size.h - SHOP_TEXT_AREA_HEIGHT), window, s_menu_sections, 1, NULL);
    layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
    
    s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COOKIE_FONT_14));

    // TODO for some reason on gabbro this doesn't wrap to the next line???
    s_cps_layer = text_layer_create(GRect(bounds.origin.x, bounds.origin.y + SHOP_TEXT_AREA_Y, bounds.size.w, SHOP_TEXT_AREA_HEIGHT));
    text_layer_set_font(s_cps_layer, s_font);
    text_layer_set_background_color(s_cps_layer, GColorClear);
    text_layer_set_text_color(s_cps_layer, GColorBlack);
    text_layer_set_text_alignment(s_cps_layer, GTextAlignmentCenter);
    text_layer_set_text(s_cps_layer, s_text);
    text_layer_set_overflow_mode(s_cps_layer, GTextOverflowModeWordWrap);
    layer_add_child(window_layer, text_layer_get_layer(s_cps_layer));
}

static void window_unload(Window *window)
{
    simple_menu_layer_destroy(s_simple_menu_layer);
    s_simple_menu_layer = NULL;

    fonts_unload_custom_font(s_font);
    s_font = NULL;

    text_layer_destroy(s_cps_layer);
    s_cps_layer = NULL;

    for (int i = 0; i < NUM_BUILDINGS; i++)
    {
        if (s_first_menu_items[i].icon != NULL)
        {
            gbitmap_destroy(s_first_menu_items[i].icon);
            s_first_menu_items[i].icon = NULL;
        }

        free(s_option_titles[i]);
        s_option_titles[i] = NULL;
        free(s_option_subtitles[i]);
        s_option_subtitles[i] = NULL;
    }
}

void shop_window_push()
{
    if (!s_window)
    {
        s_window = window_create();
        window_set_window_handlers(s_window, (WindowHandlers){
                                                 .load = window_load,
                                                 .unload = window_unload,
                                             });
    }
    window_stack_push(s_window, true);
}

void shop_window_free()
{
    window_destroy(s_window);
}

void shop_on_cookie_change()
{
    if (!window_stack_contains_window(s_window))
    {
        return;
    }

    update_menu();
    layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
    layer_mark_dirty(text_layer_get_layer(s_cps_layer));
}
