#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/pulse.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_memory.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * Constants
 *******************************************************************************
 */

/* These are intended as regression tests to catch accidental modification of
 * the constant values */

TEST test_constant_max_channels()
{
    ASSERT_EQ_FMT(4, EAARLIO_MAX_RX_COUNT, "%d");
    PASS();
}

SUITE(suite_constants)
{
    RUN_TEST(test_constant_max_channels);
}

/*******************************************************************************
 * eaarlio_pulse_free
 *******************************************************************************
 */

TEST test_free_sanity()
{
    eaarlio_pulse_free(NULL, NULL);
    PASS();
}

TEST test_free_null_pulse()
{
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_pulse_free(NULL, NULL));
    PASS();
}

TEST test_free_mem_invalid(struct eaarlio_pulse *pulse,
    struct eaarlio_memory *memory)
{
    int rx_count = pulse->rx_count;

    ASSERT(pulse->tx);
    ASSERT(pulse->rx);
    ASSERT(pulse->rx[0]);
    ASSERT_EAARLIO_ERR(
        EAARLIO_MEMORY_INVALID, eaarlio_pulse_free(pulse, memory));
    ASSERT(pulse->tx);
    ASSERT(pulse->rx[0]);
    ASSERT_EQ_FMT(rx_count, pulse->rx_count, "%d");
    PASS();
}

TEST test_free_alloc(char const *msg,
    struct eaarlio_pulse *pulse,
    struct eaarlio_memory *memory,
    struct mock_memory *mock)
{
    int rx_count = pulse->rx_count;

    ASSERT_EAARLIO_SUCCESSm(msg, eaarlio_pulse_free(pulse, memory));
    ASSERT_FALSEm(msg, pulse->tx);
    ASSERT_FALSEm(msg, pulse->rx[0]);
    ASSERT_FALSEm(msg, pulse->rx[1]);
    ASSERT_FALSEm(msg, pulse->rx[2]);
    ASSERT_FALSEm(msg, pulse->rx[3]);
    ASSERT_EQ_FMTm(msg, rx_count, pulse->rx_count, "%d");
    ASSERT_EQ_FMTm(msg, 0, mock_memory_count_in_use(mock), "%d");
    PASS();
}

SUITE(suite_free)
{
    struct mock_memory mock;
    struct eaarlio_memory memory = eaarlio_memory_empty();
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();

    RUN_TEST(test_free_sanity);
    RUN_TEST(test_free_null_pulse);

    pulse.tx = malloc(1);
    pulse.rx[0] = malloc(1);
    assert(pulse.tx);
    assert(pulse.rx[0]);
    RUN_TESTp(test_free_mem_invalid, &pulse, &memory);
    free(pulse.tx);
    free(pulse.rx[0]);

    pulse = eaarlio_pulse_empty();
    mock_memory_new(&memory, &mock, 0);
    RUN_TESTp(test_free_alloc, "no alloc", &pulse, &memory, &mock);

    mock_memory_reset(&mock, 1);
    pulse = eaarlio_pulse_empty();
    pulse.tx = memory.malloc(&memory, 1);
    assert(pulse.tx);
    RUN_TESTp(test_free_alloc, "alloc tx", &pulse, &memory, &mock);

    mock_memory_reset(&mock, 1);
    pulse = eaarlio_pulse_empty();
    pulse.rx_count = 1;
    pulse.rx[0] = memory.malloc(&memory, 1);
    assert(pulse.rx[0]);
    RUN_TESTp(test_free_alloc, "alloc rx[0]", &pulse, &memory, &mock);

    mock_memory_reset(&mock, 1);
    pulse = eaarlio_pulse_empty();
    pulse.rx_count = 4;
    pulse.rx[1] = memory.malloc(&memory, 1);
    assert(pulse.rx[1]);
    RUN_TESTp(test_free_alloc, "alloc rx[1]", &pulse, &memory, &mock);

    mock_memory_reset(&mock, 1);
    pulse = eaarlio_pulse_empty();
    pulse.rx_count = 1;
    pulse.rx[2] = memory.malloc(&memory, 1);
    assert(pulse.rx[2]);
    RUN_TESTp(
        test_free_alloc, "alloc rx[2], count = 1", &pulse, &memory, &mock);

    mock_memory_reset(&mock, 3);
    pulse = eaarlio_pulse_empty();
    pulse.rx_count = 4;
    pulse.tx = memory.malloc(&memory, 1);
    pulse.rx[2] = memory.malloc(&memory, 1);
    pulse.rx[3] = memory.malloc(&memory, 1);
    assert(pulse.tx);
    assert(pulse.rx[2]);
    assert(pulse.rx[3]);
    RUN_TESTp(
        test_free_alloc, "alloc tx, rx[2], rx[3]", &pulse, &memory, &mock);

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

    RUN_SUITE(suite_constants);
    RUN_SUITE(suite_free);

    GREATEST_MAIN_END();
}
