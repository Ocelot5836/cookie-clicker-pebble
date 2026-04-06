#include "buildings.h"
#include "../storage/storage.h"
#include <math.h>

const uint64_t s_powers[] = {
    115,
    100,
    13225,
    10000
};

void building_set_cost(BuildingType type, uint32_t building_count, BigInt_t *store)
{
    BigInt_t *tmp1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *tmp2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *tmp3 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);

    switch (type)
    {
    case BUILDING_TYPE_CURSOR:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, CURSOR_BASE_COST);
        break;
    case BUILDING_TYPE_GRANDMA:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, GRANDMA_BASE_COST);
        break;
    case BUILDING_TYPE_FARM:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, FARM_BASE_COST);
        break;
    case BUILDING_TYPE_MINE:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, MINE_BASE_COST);
        break;
    case BUILDING_TYPE_FACTORY:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, FACTORY_BASE_COST);
        break;
    case BUILDING_TYPE_BANK:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, BANK_BASE_COST);
        break;
    case BUILDING_TYPE_TEMPLE:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, TEMPLE_BASE_COST);
        break;
    case BUILDING_TYPE_WIZARD_TOWER:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, WIZARD_TOWER_BASE_COST);
        break;
    case BUILDING_TYPE_SHIPMENT:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, SHIPMENT_BASE_COST);
        break;
    case BUILDING_TYPE_ALCHEMY_LAB:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, ALCHEMY_LAB_BASE_COST);
        break;
    case BUILDING_TYPE_PORTAL:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, PORTAL_BASE_COST);
        break;
    case BUILDING_TYPE_TIME_MACHINE:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, TIME_MACHINE_BASE_COST);
        break;
    case BUILDING_TYPE_ANTIMATTER_CONDENSER:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, ANTIMATTER_CONDENSER_BASE_COST);
        break;
    case BUILDING_TYPE_PRISM:
        BigInt_from_int(COOKIE_COUNTER_WORDS, store, PRISM_BASE_COST);
        break;
    case BUILDING_TYPE_CHANCEMAKER:
        BigInt_from_int(2, tmp1, QUADRILLION);
        BigInt_from_int(2, tmp2, CHANCEMAKER_BASE_COST);
        BigInt_mul(2, tmp1, 2, tmp2, COOKIE_COUNTER_WORDS, store);
        break;
    case BUILDING_TYPE_FRACTAL_ENGINE:
        BigInt_from_int(2, tmp1, QUADRILLION);
        BigInt_from_int(2, tmp2, FRACTAL_ENGINE_BASE_COST);
        BigInt_mul(2, tmp1, 2, tmp2, COOKIE_COUNTER_WORDS, store);
        break;
    case BUILDING_TYPE_JAVASCRIPT_CONSOLE:
        BigInt_from_int(2, tmp1, QUADRILLION);
        BigInt_from_int(2, tmp2, JAVASCRIPT_CONSOLE_BASE_COST);
        BigInt_mul(2, tmp1, 2, tmp2, COOKIE_COUNTER_WORDS, store);
        break;
    case BUILDING_TYPE_IDLEVERSE:
        BigInt_from_int(2, tmp1, QUADRILLION);
        BigInt_from_int(2, tmp2, IDLEVERSE_BASE_COST);
        BigInt_mul(2, tmp1, 2, tmp2, COOKIE_COUNTER_WORDS, store);
        break;
    case BUILDING_TYPE_CORTEX_BAKER:
        BigInt_from_int(2, tmp1, QUADRILLION);
        BigInt_from_int(2, tmp2, CORTEX_BAKER_BASE_COST);
        BigInt_mul(2, tmp1, 2, tmp2, COOKIE_COUNTER_WORDS, store);
        break;
    case BUILDING_TYPE_YOU:
        BigInt_from_int(2, tmp1, QUADRILLION);
        BigInt_from_int(2, tmp2, YOU_BASE_COST);
        BigInt_mul(2, tmp1, 2, tmp2, COOKIE_COUNTER_WORDS, store);
        break;
    default:
        break;
    }

    BigInt_from_int(COOKIE_COUNTER_WORDS, tmp1, 115);
    BigInt_from_int(COOKIE_COUNTER_WORDS, tmp2, building_count);
    
    BigInt_pow(COOKIE_COUNTER_WORDS, tmp1, tmp2, tmp3);
    BigInt_mul_basic(COOKIE_COUNTER_WORDS, tmp3, store, tmp1);

    BigInt_from_int(COOKIE_COUNTER_WORDS, tmp3, 100);
    BigInt_from_int(COOKIE_COUNTER_WORDS, store, building_count);
    BigInt_pow(COOKIE_COUNTER_WORDS, tmp3, store, tmp2);

    BigInt_div(COOKIE_COUNTER_WORDS, tmp1, tmp2, store);

    free(tmp1);
    free(tmp2);
    free(tmp3);
}