#include "storage.h"

#include "../buildings/buildings.h"

static void storage_migrate()
{
    int last_storage_version = persist_read_int(DATA_STORAGE_VERSION);

    if (last_storage_version == STORAGE_VERSION)
    {
        return;
    }

    //   // Migrate data
    //   switch(last_storage_version) {
    //     case 0:
    //       // ...
    //       break;
    //     case 1:
    //       // ...
    //       break;
    //     case 2: {
    //       uint32_t old_highscore_key = 12;
    //       uint32_t new_highscore_key = 13;

    //       // Migrate to scheme version 3
    //       int highscore = persist_read_int(old_highscore_key);
    //       persist_write_int(new_highscore_key, highscore);

    //       // Delete old data
    //       persist_delete(old_highscore_key);
    //       break;
    //   }

    // Migration is complete, store the current storage scheme version number
    persist_write_int(DATA_STORAGE_VERSION, STORAGE_VERSION);
}

void storage_write_cookies(BigInt_t *cookie_count)
{
    persist_write_data(DATA_COOKIE_COUNT, cookie_count, BigIntWordSize * COOKIE_COUNTER_WORDS);
}

void storage_write_buildings(uint8_t *building_counts)
{
    persist_write_data(DATA_BUILDING_COUNTS, building_counts, NUM_BUILDINGS * sizeof(uint8_t));
}

void storage_read_cookies(BigInt_t *cookie_count)
{
    storage_migrate();

    if (persist_read_data(DATA_COOKIE_COUNT, cookie_count, BigIntWordSize * COOKIE_COUNTER_WORDS) == E_DOES_NOT_EXIST)
    {
        BigInt_zero(COOKIE_COUNTER_WORDS, cookie_count);
    }
}

void storage_read_buildings(uint8_t *building_counts)
{
    storage_migrate();

    if (persist_read_data(DATA_BUILDING_COUNTS, building_counts, NUM_BUILDINGS * sizeof(uint8_t)) == E_DOES_NOT_EXIST)
    {
        memset(building_counts, 0, NUM_BUILDINGS * sizeof(uint8_t));
    }
}