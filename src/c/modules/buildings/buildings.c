#include "buildings.h"
#include "../storage/storage.h"
#include <math.h>

static BigInt_t *s_building_costs[NUM_BUILDINGS];
static BigInt_t *factor;
static BigInt_t *temp1;
static BigInt_t *temp2;
static uint64_t s_building_cpt[] = {
    TPS / 10uLL,
    TPS,
    TPS * 8uLL,
    TPS * 47uLL,
    TPS * 260uLL,
    TPS * 1400uLL,
    TPS * 7800uLL,
    TPS * 44000uLL,
    TPS * 260000uLL,
    TPS * 1600000uLL,
    TPS * 10uLL * MILLION,
    TPS * 65uLL * MILLION,
    TPS * 430uLL * MILLION,
    TPS * 2900uLL * MILLION,
    TPS * 21uLL * BILLION,
    TPS * 150uLL * BILLION,
    TPS * 1100uLL * BILLION,
    TPS * 8300uLL * BILLION,
    TPS * 64uLL * TRILLION,
    TPS * 510uLL * TRILLION,
};

static void building_set_cost(BuildingType type, uint32_t building_count, BigInt_t *store, BigInt_t *tmp1, BigInt_t *tmp2, BigInt_t *tmp3)
{
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
}

void buildings_init(uint8_t *building_counts)
{
    BigInt_t *tmp1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *tmp2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *tmp3 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);

    factor = malloc(BigIntWordSize * 2);
    temp1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    temp2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        s_building_costs[i] = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
        building_set_cost(BUILDING_TYPE_CURSOR + i, building_counts[i], s_building_costs[i], tmp1, tmp2, tmp3);
    }

    free(tmp1);
    free(tmp2);
    free(tmp3);
}

void buildings_free()
{
    free(factor);
    free(temp1);
    free(temp2);
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        free(s_building_costs[i]);
    }
}

BigInt_t *building_update_cost(BuildingType type, uint8_t building_count)
{
    BigInt_t *tmp1 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *tmp2 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);
    BigInt_t *tmp3 = malloc(BigIntWordSize * COOKIE_COUNTER_WORDS);

    building_set_cost(type, building_count, s_building_costs[type], tmp1, tmp2, tmp3);

    free(tmp1);
    free(tmp2);
    free(tmp3);

    return s_building_costs[type];
}

BigInt_t *building_get_cost(BuildingType type)
{
    return s_building_costs[type];
}

void building_get_cpt(uint8_t *building_counts, BigInt_t *store)
{
    BigInt_zero(COOKIE_COUNTER_WORDS, store);
    for (size_t i = 0; i < NUM_BUILDINGS; i++)
    {
        if (building_counts[i] == 0)
        {
            continue;
        }
        
        BigInt_from_int(2, factor, building_counts[i]);
        BigInt_from_int(COOKIE_COUNTER_WORDS, temp1, s_building_cpt[i]);
        BigInt_mul(2, factor, COOKIE_COUNTER_WORDS, temp1, COOKIE_COUNTER_WORDS, temp2);
        BigInt_add(COOKIE_COUNTER_WORDS, store, COOKIE_COUNTER_WORDS, temp2, COOKIE_COUNTER_WORDS, temp1);
        BigInt_copy(COOKIE_COUNTER_WORDS, store, temp1);
    }
}
