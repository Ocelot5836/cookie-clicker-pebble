#include "storage.h"

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

void storage_write(BigInt_t *cookie_count)
{
    persist_write_data(DATA_COOKIE_COUNT, cookie_count, BigIntWordSize * COOKIE_COUNTER_WORDS);
}

void storage_read(BigInt_t *cookie_count)
{
    storage_migrate();

    persist_read_data(DATA_COOKIE_COUNT, cookie_count, BigIntWordSize * COOKIE_COUNTER_WORDS);
}