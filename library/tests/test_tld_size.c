#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_size.h"
#include "greatest.h"
#include "assert_error.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "data_tld.c"

#ifdef _MSC_VER
/* Visual Studio complains about initializing structs with references to
 * non-constant values because it's non-standard, but it's supported on all
 * current target platforms */
#pragma warning(disable : 4204)
/* Visual Studio complains about initializing structs with references to local
 * variables because it's non-standard, but it's supported on all current
 * target platforms */
#pragma warning(disable : 4221)
#endif

/*******************************************************************************
 * eaarlio_tld_size_pulse_waveforms
 *******************************************************************************
 */

/* Make sure we don't seg fault */
TEST test_size_pulse_waveforms_sanity()
{
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();
    uint16_t size;
    eaarlio_tld_size_pulse_waveforms(&pulse, &size);
    PASS();
}

/* Can we handle a null pulse? */
TEST test_size_pulse_waveforms_null_pulse()
{
    uint16_t size;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_size_pulse_waveforms(NULL, &size));
    PASS();
}

/* Can we handle a null size? */
TEST test_size_pulse_waveforms_null_size()
{
    struct eaarlio_pulse pulse;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_size_pulse_waveforms(&pulse, NULL));
    PASS();
}

/* Can we handle normal values?
 * Do we ignore extra rx lengths beyond rx_count?
 */
TEST test_size_pulse_waveforms_basic()
{
    struct eaarlio_pulse pulse;
    uint16_t actual = 0;
    uint16_t expected = (EAARLIO_TLD_TX_LENGTH_SIZE + 5
        + EAARLIO_TLD_RX_LENGTH_SIZE + 6 + EAARLIO_TLD_RX_LENGTH_SIZE + 7);

    memset(&pulse, 0, sizeof pulse);
    pulse.tx = NULL;
    pulse.rx[0] = NULL;
    pulse.rx[1] = NULL;
    pulse.rx[2] = NULL;
    pulse.rx[3] = NULL;

    pulse.rx_count = 2;
    pulse.tx_len = 5;
    pulse.rx_len[0] = 6;
    pulse.rx_len[1] = 7;
    pulse.rx_len[2] = 8;
    pulse.rx_len[3] = 9;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_size_pulse_waveforms(&pulse, &actual));
    ASSERT_EQ_FMT(expected, actual, "%d");
    PASS();
}

/* Can we handle zero waveforms? */
TEST test_size_pulse_waveforms_zero_wf()
{
    struct eaarlio_pulse pulse;
    uint16_t actual = 0;
    uint16_t expected = (EAARLIO_TLD_TX_LENGTH_SIZE + 5);

    memset(&pulse, 0, sizeof pulse);
    pulse.tx = NULL;
    pulse.rx[0] = NULL;
    pulse.rx[1] = NULL;
    pulse.rx[2] = NULL;
    pulse.rx[3] = NULL;

    pulse.rx_count = 0;
    pulse.tx_len = 5;
    pulse.rx_len[0] = 6;
    pulse.rx_len[1] = 7;
    pulse.rx_len[2] = 8;
    pulse.rx_len[3] = 9;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_size_pulse_waveforms(&pulse, &actual));
    ASSERT_EQ_FMT(expected, actual, "%d");
    PASS();
}

/* Can we handle too many waveforms? */
TEST test_size_pulse_waveforms_extra_wf()
{
    struct eaarlio_pulse pulse;
    uint16_t actual = 0;
    uint16_t expected = (EAARLIO_TLD_TX_LENGTH_SIZE + 5
        + EAARLIO_TLD_RX_LENGTH_SIZE + 6 + EAARLIO_TLD_RX_LENGTH_SIZE + 7
        + EAARLIO_TLD_RX_LENGTH_SIZE + 8 + EAARLIO_TLD_RX_LENGTH_SIZE + 9);

    memset(&pulse, 0, sizeof pulse);
    pulse.tx = NULL;
    pulse.rx[0] = NULL;
    pulse.rx[1] = NULL;
    pulse.rx[2] = NULL;
    pulse.rx[3] = NULL;

    pulse.rx_count = EAARLIO_MAX_RX_COUNT + 2;
    pulse.tx_len = 5;
    pulse.rx_len[0] = 6;
    pulse.rx_len[1] = 7;
    pulse.rx_len[2] = 8;
    pulse.rx_len[3] = 9;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_size_pulse_waveforms(&pulse, &actual));
    ASSERT_EQ_FMT(expected, actual, "%d");
    PASS();
}

/* Can we handle data that's too big? */
TEST test_size_pulse_waveforms_too_big()
{
    struct eaarlio_pulse pulse;
    uint16_t actual = 0;

    memset(&pulse, 0, sizeof pulse);
    pulse.tx = NULL;
    pulse.rx[0] = NULL;
    pulse.rx[1] = NULL;
    pulse.rx[2] = NULL;
    pulse.rx[3] = NULL;

    pulse.rx_count = 4;
    pulse.tx_len = 255;
    pulse.rx_len[0] = 65535;
    pulse.rx_len[1] = 65535;
    pulse.rx_len[2] = 65535;
    pulse.rx_len[3] = 65535;

    ASSERT_EAARLIO_ERR(EAARLIO_VALUE_OUT_OF_RANGE,
        eaarlio_tld_size_pulse_waveforms(&pulse, &actual));
    ASSERT_EQ_FMT(0, actual, "%d");
    PASS();
}

/* Can we handle zero length waveforms? */
TEST test_size_pulse_waveforms_zero_len()
{
    struct eaarlio_pulse pulse;
    uint16_t actual = 0;
    uint16_t expected = (EAARLIO_TLD_TX_LENGTH_SIZE + 0
        + EAARLIO_TLD_RX_LENGTH_SIZE + 0 + EAARLIO_TLD_RX_LENGTH_SIZE + 0
        + EAARLIO_TLD_RX_LENGTH_SIZE + 0 + EAARLIO_TLD_RX_LENGTH_SIZE + 0);

    memset(&pulse, 0, sizeof pulse);
    pulse.tx = NULL;
    pulse.rx[0] = NULL;
    pulse.rx[1] = NULL;
    pulse.rx[2] = NULL;
    pulse.rx[3] = NULL;

    pulse.rx_count = 4;
    pulse.tx_len = 0;
    pulse.rx_len[0] = 0;
    pulse.rx_len[1] = 0;
    pulse.rx_len[2] = 0;
    pulse.rx_len[3] = 0;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_size_pulse_waveforms(&pulse, &actual));
    ASSERT_EQ_FMT(expected, actual, "%d");
    PASS();
}

SUITE(suite_size_pulse_waveforms)
{
    RUN_TEST(test_size_pulse_waveforms_sanity);
    RUN_TEST(test_size_pulse_waveforms_null_pulse);
    RUN_TEST(test_size_pulse_waveforms_null_size);
    RUN_TEST(test_size_pulse_waveforms_basic);
    RUN_TEST(test_size_pulse_waveforms_zero_wf);
    RUN_TEST(test_size_pulse_waveforms_extra_wf);
    RUN_TEST(test_size_pulse_waveforms_too_big);
    RUN_TEST(test_size_pulse_waveforms_zero_len);
}

/*******************************************************************************
 * eaarlio_tld_size_pulse
 *******************************************************************************
 */

/* Make sure we don't seg fault */
TEST test_size_pulse_sanity()
{
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();
    uint32_t size;
    eaarlio_tld_size_pulse(&pulse, &size);
    PASS();
}

/* Can we handle a null pulse? */
TEST test_size_pulse_null_pulse()
{
    uint32_t size;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_tld_size_pulse(NULL, &size));
    PASS();
}

/* Can we handle a null size? */
TEST test_size_pulse_null_size()
{
    struct eaarlio_pulse pulse;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_tld_size_pulse(&pulse, NULL));
    PASS();
}

/* Can we handle normal values? */
TEST test_size_pulse_basic()
{
    struct eaarlio_pulse pulse;
    uint32_t actual = 0U;
    uint16_t expected = (EAARLIO_TLD_PULSE_HEADER_SIZE
        + EAARLIO_TLD_WF_DATA_LENGTH_SIZE + EAARLIO_TLD_TX_LENGTH_SIZE + 5
        + EAARLIO_TLD_RX_LENGTH_SIZE + 6 + EAARLIO_TLD_RX_LENGTH_SIZE + 7);

    memset(&pulse, 0, sizeof pulse);
    pulse.tx = NULL;
    pulse.rx[0] = NULL;
    pulse.rx[1] = NULL;
    pulse.rx[2] = NULL;
    pulse.rx[3] = NULL;

    pulse.rx_count = 2;
    pulse.tx_len = 5;
    pulse.rx_len[0] = 6;
    pulse.rx_len[1] = 7;
    pulse.rx_len[2] = 8;
    pulse.rx_len[3] = 9;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_size_pulse(&pulse, &actual));
    ASSERT_EQ_FMT(expected, actual, "%d");
    PASS();
}

SUITE(suite_size_pulse)
{
    RUN_TEST(test_size_pulse_sanity);
    RUN_TEST(test_size_pulse_null_pulse);
    RUN_TEST(test_size_pulse_null_size);
    RUN_TEST(test_size_pulse_basic);
}

/*******************************************************************************
 * eaarlio_tld_size_raster_pulses
 *******************************************************************************
 */

/* Make sure we don't seg fault */
TEST test_size_raster_pulses_sanity()
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    uint32_t size;
    eaarlio_tld_size_raster_pulses(&raster, &size);
    PASS();
}

/* Can we handle a null raster? */
TEST test_size_raster_pulses_null_raster()
{
    uint32_t size;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_size_raster_pulses(NULL, &size));
    PASS();
}

/* Can we handle a null size? */
TEST test_size_raster_pulses_null_size()
{
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_size_raster_pulses(&raster, NULL));
    PASS();
}

/* Can we handle normal values? */
TEST test_size_raster_pulses_basic()
{
    struct eaarlio_raster raster;
    struct eaarlio_pulse pulse[2];
    uint32_t actual = 0U;
    uint32_t expected = (EAARLIO_TLD_PULSE_HEADER_SIZE
        + EAARLIO_TLD_WF_DATA_LENGTH_SIZE + EAARLIO_TLD_TX_LENGTH_SIZE + 10
        + EAARLIO_TLD_RX_LENGTH_SIZE + 11 + EAARLIO_TLD_RX_LENGTH_SIZE + 12
        + EAARLIO_TLD_RX_LENGTH_SIZE + 13 + EAARLIO_TLD_PULSE_HEADER_SIZE
        + EAARLIO_TLD_WF_DATA_LENGTH_SIZE + EAARLIO_TLD_TX_LENGTH_SIZE + 20
        + EAARLIO_TLD_RX_LENGTH_SIZE + 21 + EAARLIO_TLD_RX_LENGTH_SIZE + 22);

    memset(&raster, 0, sizeof raster);
    memset(pulse, 0, sizeof pulse);

    pulse[0].tx = NULL;
    pulse[0].rx[0] = NULL;
    pulse[0].rx[1] = NULL;
    pulse[0].rx[2] = NULL;
    pulse[0].rx[3] = NULL;
    pulse[1].tx = NULL;
    pulse[1].rx[0] = NULL;
    pulse[1].rx[1] = NULL;
    pulse[1].rx[2] = NULL;
    pulse[1].rx[3] = NULL;

    raster.pulse_count = 2;
    raster.pulse = (struct eaarlio_pulse *)pulse;

    pulse[0].rx_count = 3;
    pulse[0].tx_len = 10;
    pulse[0].rx_len[0] = 11;
    pulse[0].rx_len[1] = 12;
    pulse[0].rx_len[2] = 13;

    pulse[1].rx_count = 2;
    pulse[1].tx_len = 20;
    pulse[1].rx_len[0] = 21;
    pulse[1].rx_len[1] = 22;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_size_raster_pulses(&raster, &actual));
    ASSERT_EQ_FMT(expected, actual, "%d");
    PASS();
}

SUITE(suite_size_raster_pulses)
{
    RUN_TEST(test_size_raster_pulses_sanity);
    RUN_TEST(test_size_raster_pulses_null_raster);
    RUN_TEST(test_size_raster_pulses_null_size);
    RUN_TEST(test_size_raster_pulses_basic);
}

/*******************************************************************************
 * eaarlio_tld_size_raster
 *******************************************************************************
 */

/* Make sure we don't seg fault */
TEST test_size_raster_sanity()
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    uint32_t size;
    eaarlio_tld_size_raster(&raster, &size);
    PASS();
}

/* Can we handle a null raster? */
TEST test_size_raster_null_raster()
{
    uint32_t size;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_tld_size_raster(NULL, &size));
    PASS();
}

/* Can we handle a null size? */
TEST test_size_raster_null_size()
{
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_tld_size_raster(&raster, NULL));
    PASS();
}

/* Can we handle normal values? */
TEST test_size_raster_basic()
{
    struct eaarlio_raster raster;
    struct eaarlio_pulse pulse[2];
    uint32_t actual = 0U;
    uint32_t expected = (EAARLIO_TLD_RASTER_HEADER_SIZE
        + EAARLIO_TLD_PULSE_HEADER_SIZE + EAARLIO_TLD_WF_DATA_LENGTH_SIZE
        + EAARLIO_TLD_TX_LENGTH_SIZE + 10 + EAARLIO_TLD_RX_LENGTH_SIZE + 11
        + EAARLIO_TLD_RX_LENGTH_SIZE + 12 + EAARLIO_TLD_RX_LENGTH_SIZE + 13
        + EAARLIO_TLD_PULSE_HEADER_SIZE + EAARLIO_TLD_WF_DATA_LENGTH_SIZE
        + EAARLIO_TLD_TX_LENGTH_SIZE + 20 + EAARLIO_TLD_RX_LENGTH_SIZE + 21
        + EAARLIO_TLD_RX_LENGTH_SIZE + 22);

    memset(&raster, 0, sizeof raster);
    memset(pulse, 0, sizeof pulse);

    pulse[0].tx = NULL;
    pulse[0].rx[0] = NULL;
    pulse[0].rx[1] = NULL;
    pulse[0].rx[2] = NULL;
    pulse[0].rx[3] = NULL;
    pulse[1].tx = NULL;
    pulse[1].rx[0] = NULL;
    pulse[1].rx[1] = NULL;
    pulse[1].rx[2] = NULL;
    pulse[1].rx[3] = NULL;

    raster.pulse_count = 2;
    raster.pulse = (struct eaarlio_pulse *)pulse;

    pulse[0].rx_count = 3;
    pulse[0].tx_len = 10;
    pulse[0].rx_len[0] = 11;
    pulse[0].rx_len[1] = 12;
    pulse[0].rx_len[2] = 13;

    pulse[1].rx_count = 2;
    pulse[1].tx_len = 20;
    pulse[1].rx_len[0] = 21;
    pulse[1].rx_len[1] = 22;

    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_size_raster(&raster, &actual));
    ASSERT_EQ_FMT(expected, actual, "%d");
    PASS();
}

TEST test_size_raster_pulse_err()
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    uint32_t actual = 1U;

    raster.pulse_count = 10;

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_tld_size_raster(&raster, &actual));
    ASSERT_EQ_FMT(0, actual, "%d");
    PASS();
}

SUITE(suite_size_raster)
{
    RUN_TEST(test_size_raster_sanity);
    RUN_TEST(test_size_raster_null_raster);
    RUN_TEST(test_size_raster_null_size);
    RUN_TEST(test_size_raster_basic);
    RUN_TEST(test_size_raster_pulse_err);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_size_pulse_waveforms);
    RUN_SUITE(suite_size_pulse);
    RUN_SUITE(suite_size_raster_pulses);
    RUN_SUITE(suite_size_raster);

    GREATEST_MAIN_END();
}
