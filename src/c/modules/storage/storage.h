#pragma once

#include <pebble.h>
#include "../bigint/BigInt.h"

#define STORAGE_VERSION 0
#define DATA_STORAGE_VERSION 0
#define DATA_COOKIE_COUNT 1
#define DATA_BUILDING_COUNTS 2
#define DATA_LAST_PLAYED_TIME 3
#define COOKIE_COUNTER_WORDS 32

void storage_write_cookies(BigInt_t *cookie_count);

void storage_write_buildings(uint8_t *building_counts);

void storage_write_time(time_t *epoch_time);

void storage_remove_time();

void storage_read_cookies(BigInt_t *cookie_count);

void storage_read_buildings(uint8_t *building_counts);

void storage_read_time(time_t *epoch_time);
