#include "tempnumbers.h"
#include "storage.h"

BigInt_t *number_temp1;
BigInt_t *number_temp2;
BigInt_t *number_temp3;
BigInt_t *number_temp4;

void tempnumbers_init()
{
    number_temp1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    number_temp2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    number_temp3 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    number_temp4 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
}

void tempnumbers_free()
{
    free(number_temp1);
    free(number_temp2);
    free(number_temp3);
    free(number_temp4);
}
