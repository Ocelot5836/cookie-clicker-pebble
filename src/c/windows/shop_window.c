#include "shop_window.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 7
#define MENU_SPRITE_SIZE 32

static Window *s_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_MENU_ITEMS];
static GBitmap *s_menu_icon_buildings;
static GBitmap *s_menu_icon_cursor;
static GBitmap *s_menu_icon_grandma;
static GBitmap *s_menu_icon_farm;
static GBitmap *s_menu_icon_mine;
static GBitmap *s_menu_icon_factory;
static GBitmap *s_menu_icon_bank;
static GBitmap *s_menu_icon_temple;

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

    s_menu_icon_cursor = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, 0, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));
    s_menu_icon_grandma = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));
    s_menu_icon_farm = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, 2 * MENU_SPRITE_SIZE, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));
    s_menu_icon_mine = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, 3 * MENU_SPRITE_SIZE, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));
    s_menu_icon_factory = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, 4 * MENU_SPRITE_SIZE, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));
    s_menu_icon_bank = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, 5 * MENU_SPRITE_SIZE, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));
    s_menu_icon_temple = gbitmap_create_as_sub_bitmap(s_menu_icon_buildings, GRect(0, 6 * MENU_SPRITE_SIZE, MENU_SPRITE_SIZE, MENU_SPRITE_SIZE));

    s_first_menu_items[0] = (SimpleMenuItem){
        .title = "Cursor",
        .subtitle = "100 million",
        .callback = menu_select_callback,
        .icon = s_menu_icon_cursor,
    };
    s_first_menu_items[1] = (SimpleMenuItem){
        .title = "Grandma",
        .callback = menu_select_callback,
        .icon = s_menu_icon_grandma,
    };
    s_first_menu_items[2] = (SimpleMenuItem){
        .title = "Farm",
        .callback = menu_select_callback,
        .icon = s_menu_icon_farm,
    };
    s_first_menu_items[3] = (SimpleMenuItem){
        .title = "Mine",
        .callback = menu_select_callback,
        .icon = s_menu_icon_mine,
    };
    s_first_menu_items[4] = (SimpleMenuItem){
        .title = "Factory",
        .callback = menu_select_callback,
        .icon = s_menu_icon_factory,
    };
    s_first_menu_items[5] = (SimpleMenuItem){
        .title = "Bank",
        .callback = menu_select_callback,
        .icon = s_menu_icon_bank,
    };
    s_first_menu_items[6] = (SimpleMenuItem){
        .title = "Temple",
        .callback = menu_select_callback,
        .icon = s_menu_icon_temple,
    };

    s_menu_sections[0] = (SimpleMenuSection){
        .num_items = NUM_MENU_ITEMS,
        .items = s_first_menu_items,
    };

    s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);

    layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void window_unload(Window *window)
{
    simple_menu_layer_destroy(s_simple_menu_layer);
    s_simple_menu_layer = NULL;

    gbitmap_destroy(s_menu_icon_cursor);
    s_menu_icon_cursor = NULL;

    gbitmap_destroy(s_menu_icon_grandma);
    s_menu_icon_grandma = NULL;

    gbitmap_destroy(s_menu_icon_farm);
    s_menu_icon_farm = NULL;

    gbitmap_destroy(s_menu_icon_mine);
    s_menu_icon_mine = NULL;

    gbitmap_destroy(s_menu_icon_factory);
    s_menu_icon_factory = NULL;

    gbitmap_destroy(s_menu_icon_bank);
    s_menu_icon_bank = NULL;

    gbitmap_destroy(s_menu_icon_temple);
    s_menu_icon_temple = NULL;

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
