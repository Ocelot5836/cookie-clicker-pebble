#pragma once

#include <pebble.h>
#include "../bigint/BigInt.h"

void format_init();

void format_free();

void format_cookie_number(BigInt_t *value, BigInt_t *cpt, size_t max_length, char* buffer);

uint32_t format_number(BigInt_t *value, size_t max_length, char* buffer);