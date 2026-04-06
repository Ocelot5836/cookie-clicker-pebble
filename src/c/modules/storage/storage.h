#pragma once

#include <pebble.h>
#include "../bigint/BigInt.h"

#define STORAGE_VERSION 0
#define DATA_STORAGE_VERSION 0
#define DATA_COOKIE_COUNT 1
#define COOKIE_COUNTER_WORDS 32

void storage_write(BigInt_t *cookie_count);

void storage_read(BigInt_t *cookie_count);
