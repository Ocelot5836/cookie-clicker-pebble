#pragma once

#include <pebble.h>
#include "../bigint/BigInt.h"

extern BigInt_t *number_temp1;
extern BigInt_t *number_temp2;
extern BigInt_t *number_temp3;
extern BigInt_t *number_temp4;

void tempnumbers_init();

void tempnumbers_free();
