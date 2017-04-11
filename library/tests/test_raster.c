#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/raster.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_memory.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * eaarlio_raster_free
 *******************************************************************************
 */

TEST test_free_sanity()
{
    eaarlio_raster_free(NULL, NULL);
    PASS();
}

TEST test_free_null_raster()
{
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_raster_free(NULL, NULL));
    PASS();
}

TEST test_free_mem_invalid(struct eaarlio_raster *raster,
    struct eaarlio_memory *memory)
{
    int i;
    int pulse_count = raster->pulse_count;

    ASSERT(raster->pulse);
    for(i = 0; i < pulse_count; i++)
        ASSERT(raster->pulse[i].tx);
    ASSERT_EAARLIO_ERR(
        EAARLIO_MEMORY_INVALID, eaarlio_raster_free(raster, memory));
    ASSERT(raster->pulse);
    for(i = 0; i < pulse_count; i++)
        ASSERT(raster->pulse[i].tx);
    ASSERT_EQ_FMT(pulse_count, raster->pulse_count, "%d");
    PASS();
}

TEST test_free_alloc(char const *msg,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    struct mock_memory *mock)
{
    int pulse_count = raster->pulse_count;

    ASSERT_EAARLIO_SUCCESSm(msg, eaarlio_raster_free(raster, memory));
    ASSERT_FALSEm(msg, raster->pulse);
    ASSERT_EQ_FMTm(msg, pulse_count, raster->pulse_count, "%d");
    ASSERT_EQ_FMTm(msg, 0, mock_memory_count_in_use(mock), "%d");
    PASS();
}

SUITE(suite_free)
{
    struct mock_memory mock;
    struct eaarlio_memory memory = eaarlio_memory_empty();
    struct eaarlio_raster raster = eaarlio_raster_empty();

    RUN_TEST(test_free_sanity);
    RUN_TEST(test_free_null_raster);

    raster.pulse_count = 2;
    raster.pulse = calloc(2, sizeof(struct eaarlio_pulse));
    assert(raster.pulse);
    raster.pulse[0].tx = malloc(1);
    assert(raster.pulse[0].tx);
    raster.pulse[1].tx = malloc(1);
    assert(raster.pulse[1].tx);
    RUN_TESTp(test_free_mem_invalid, &raster, &memory);
    free(raster.pulse[0].tx);
    free(raster.pulse[1].tx);
    free(raster.pulse);

    raster = eaarlio_raster_empty();
    mock_memory_new(&memory, &mock, 0);
    RUN_TESTp(test_free_alloc, "no alloc", &raster, &memory, &mock);

    mock_memory_reset(&mock, 1);
    raster = eaarlio_raster_empty();
    raster.pulse_count = 0;
    raster.pulse = memory.malloc(&memory, 1);
    assert(raster.pulse);
    RUN_TESTp(test_free_alloc, "alloc pulse, pulse_count = 0", &raster, &memory,
        &mock);

    mock_memory_reset(&mock, 1);
    raster = eaarlio_raster_empty();
    raster.pulse_count = 2;
    raster.pulse = memory.calloc(&memory, 2, sizeof(struct eaarlio_pulse));
    assert(raster.pulse);
    RUN_TESTp(test_free_alloc, "alloc pulse, no tx", &raster, &memory, &mock);

    mock_memory_reset(&mock, 3);
    raster = eaarlio_raster_empty();
    raster.pulse_count = 2;
    raster.pulse = memory.calloc(&memory, 2, sizeof(struct eaarlio_pulse));
    assert(raster.pulse);
    raster.pulse[0].tx = memory.malloc(&memory, 1);
    assert(raster.pulse[0].tx);
    raster.pulse[1].tx = memory.malloc(&memory, 1);
    assert(raster.pulse[1].tx);
    RUN_TESTp(test_free_alloc, "alloc pulse, with tx", &raster, &memory, &mock);

    mock_memory_destroy(&memory);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_free);

    GREATEST_MAIN_END();
}
