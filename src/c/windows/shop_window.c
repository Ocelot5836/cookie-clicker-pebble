#include "shop_window.h"
#include "../modules/engine/numberformat.h"
#include "../modules/buildings/buildings.h"
#include "../modules/storage/storage.h"

#define MENU_SPRITE_SIZE 32

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
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[1];
static SimpleMenuItem s_first_menu_items[NUM_BUILDINGS];

static char *s_building_values[NUM_BUILDINGS];
static GBitmap *s_menu_icon_buildings;
static GBitmap *s_menu_icon[NUM_BUILDINGS];

static void menu_select_callback(int index, void *ctx)
{
    s_first_menu_items[index].subtitle = "You've hit select here!";
    layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // TODO stream parts of the image instead of trying to load the entire image
    s_menu_icon_buildings = gbitmap_create_with_resource(RESOURCE_ID_BUILDINGS);
    if (s_menu_icon_buildings == NULL)
    {
        APP_LOG(APP_LOG_LEVEL_INFO, "NULL");
    }

    BigInt_t *tmp = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);

    for (int i = 0; i < NUM_BUILDINGS; i++)
    {
        s_menu_icon[i] = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, i * MENU_SPRITE_SIZE, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));
        s_building_values[i] = malloc(sizeof(char) * 51);
        building_set_cost(BUILDING_TYPE_CURSOR + i, 0, tmp);
        format_cookie_number(tmp, 50, s_building_values[i]);

        s_first_menu_items[i] = (SimpleMenuItem){
            .title = s_building_names[i],
            .subtitle = s_building_values[i],
            .callback = menu_select_callback,
            .icon = s_menu_icon[i],
        };

        APP_LOG(APP_LOG_LEVEL_INFO, "%d Bytes Free: %lu", i, heap_bytes_free());
    }

    free(tmp);

    s_menu_sections[0] = (SimpleMenuSection){
        .num_items = NUM_BUILDINGS,
        .items = s_first_menu_items,
    };

    s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, 1, NULL);
    layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void window_unload(Window *window)
{
    simple_menu_layer_destroy(s_simple_menu_layer);
    s_simple_menu_layer = NULL;

    for (int i = 0; i < NUM_BUILDINGS; i++)
    {
        free(s_menu_icon[i]);
        s_menu_icon[i] = NULL;
        free(s_building_values[i]);
        s_building_values[i] = NULL;
    }

    gbitmap_destroy(s_menu_icon_buildings);
    s_menu_icon_buildings = NULL;
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
