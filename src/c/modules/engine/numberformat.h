#pragma once

#include <pebble.h>
#include "../bigint/BigInt.h"

void format_init();

void format_free();

void format_cookie_number(BigInt_t *value, size_t max_length, char* buffer);

void format_number(BigInt_t *value, size_t max_length, char* buffer);