#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/units.h"
#include "greatest.h"
#include "greatest_extend.h"
#include "assert_range.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * Constants
 *******************************************************************************
 */

/* These are intended as regression tests to catch accidental modification of
 * the constant values */

TEST test_constant_scan_angle()
{
    ASSERT_IN_RANGE_POWER(4.5e-2, EAARLIO_UNITS_SCAN_ANGLE_COUNTS_DEGREES, -2);
    PASS();
}

TEST test_constant_time_fraction()
{
    ASSERT_IN_RANGE_POWER(1.6e-6, EAARLIO_UNITS_TIME_FRACTION_SECONDS, -6);
    PASS();
}

SUITE(suite_constants)
{
    RUN_TEST(test_constant_scan_angle);
    RUN_TEST(test_constant_time_fraction);
}

/*******************************************************************************
 * eaarlio_units_edb_time
 *******************************************************************************
 */

TEST test_edb_time_sanity()
{
    eaarlio_units_edb_time(NULL);
    PASS();
}

TEST test_edb_time_null()
{
    ASSERT_IN_RANGE_POWER(0., eaarlio_units_edb_time(NULL), 0);
    PASS();
}

TEST test_edb_time_data(uint32_t time_seconds,
    uint32_t time_fraction,
    double expected,
    int tolerance)
{
    struct eaarlio_edb_record record;
    record.time_seconds = time_seconds;
    record.time_fraction = time_fraction;
    ASSERT_IN_RANGE_POWER(expected, eaarlio_units_edb_time(&record), tolerance);
    PASS();
}

SUITE(suite_edb_time)
{
    RUN_TEST(test_edb_time_sanity);
    RUN_TEST(test_edb_time_null);

    RUN_TESTp(test_edb_time_data, 0, 0, 0., 0);
    RUN_TESTp(test_edb_time_data, 10, 0, 10., 1);
    RUN_TESTp(test_edb_time_data, 0, 10, 1.6e-5, -5);
    RUN_TESTp(test_edb_time_data, 10, 10, 10.000016, 1);
}

/*******************************************************************************
 * eaarlio_units_raster_time
 *******************************************************************************
 */

TEST test_raster_time_sanity()
{
    eaarlio_units_raster_time(NULL);
    PASS();
}

TEST test_raster_time_null()
{
    ASSERT_IN_RANGE_POWER(0., eaarlio_units_raster_time(NULL), 0);
    PASS();
}

TEST test_raster_time_data(uint32_t time_seconds,
    uint32_t time_fraction,
    double expected,
    int tolerance)
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    raster.time_seconds = time_seconds;
    raster.time_fraction = time_fraction;
    ASSERT_IN_RANGE_POWER(
        expected, eaarlio_units_raster_time(&raster), tolerance);
    PASS();
}

SUITE(suite_raster_time)
{
    RUN_TEST(test_raster_time_sanity);
    RUN_TEST(test_raster_time_null);

    RUN_TESTp(test_raster_time_data, 0, 0, 0., 0);
    RUN_TESTp(test_raster_time_data, 10, 0, 10., 1);
    RUN_TESTp(test_raster_time_data, 0, 10, 1.6e-5, -5);
    RUN_TESTp(test_raster_time_data, 10, 10, 10.000016, 1);
}

/*******************************************************************************
 * eaarlio_units_pulse_time
 *******************************************************************************
 */

TEST test_pulse_time_sanity()
{
    eaarlio_units_pulse_time(NULL, 0);
    PASS();
}

TEST test_pulse_time_null_raster()
{
    ASSERT_IN_RANGE_POWER(0., eaarlio_units_pulse_time(NULL, 0), 0);
    PASS();
}

TEST test_pulse_time_null_pulse()
{
    struct eaarlio_raster raster = eaarlio_raster_empty();

    raster.time_seconds = 1;
    raster.time_fraction = 1;
    raster.pulse_count = 1;
    raster.pulse = NULL;

    ASSERT_IN_RANGE_POWER(0., eaarlio_units_pulse_time(&raster, 1), 0);
    PASS();
}

TEST test_pulse_time_count_0()
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();

    raster.time_seconds = 1;
    raster.time_fraction = 1;
    raster.pulse_count = 0;
    raster.pulse = &pulse;

    pulse.time_offset = 1;

    ASSERT_IN_RANGE_POWER(0., eaarlio_units_pulse_time(&raster, 1), 0);
    PASS();
}

TEST test_pulse_time_number_0()
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();

    raster.time_seconds = 1;
    raster.time_fraction = 1;
    raster.pulse_count = 1;
    raster.pulse = &pulse;

    pulse.time_offset = 1;

    ASSERT_IN_RANGE_POWER(0., eaarlio_units_pulse_time(&raster, 0), 0);
    PASS();
}

TEST test_pulse_time_number_high()
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();

    raster.time_seconds = 1;
    raster.time_fraction = 1;
    raster.pulse_count = 1;
    raster.pulse = &pulse;

    pulse.time_offset = 1;

    ASSERT_IN_RANGE_POWER(0., eaarlio_units_pulse_time(&raster, 2), 0);
    PASS();
}

TEST test_pulse_time_data(char *msg,
    uint16_t pulse_number,
    uint16_t pulse_count,
    uint32_t time_seconds,
    uint32_t time_fraction,
    uint32_t time_offset,
    double expected,
    int tolerance)
{
    struct eaarlio_raster raster = eaarlio_raster_empty();
    struct eaarlio_pulse pulse[5];

    raster.time_seconds = time_seconds;
    raster.time_fraction = time_fraction;
    raster.pulse_count = pulse_count;
    raster.pulse = (struct eaarlio_pulse *)&pulse;

    pulse[pulse_number - 1].time_offset = time_offset;

    ASSERT_IN_RANGE_POWERm(msg, expected,
        eaarlio_units_pulse_time(&raster, pulse_number), tolerance);
    PASS();
}

void data_suite_pulse_time(uint16_t pulse_number, uint16_t pulse_count)
{
    RUN_MSG_INIT();
    RUN_MSG_FORMAT("[%d/%d] %d %d %d = %f ~ %d");
    RUN_MSG(test_pulse_time_data, pulse_number, pulse_count, 0, 0, 0, 0., 0);
    RUN_MSG(test_pulse_time_data, pulse_number, pulse_count, 10, 0, 0, 10., 1);
    RUN_MSG(
        test_pulse_time_data, pulse_number, pulse_count, 0, 10, 0, 1.6e-5, -5);
    RUN_MSG(
        test_pulse_time_data, pulse_number, pulse_count, 0, 0, 10, 1.6e-5, -5);
    RUN_MSG(test_pulse_time_data, pulse_number, pulse_count, 10, 10, 0,
        10.000016, 1);
    RUN_MSG(test_pulse_time_data, pulse_number, pulse_count, 10, 0, 10,
        10.000016, 1);
    RUN_MSG(test_pulse_time_data, pulse_number, pulse_count, 10, 10, 10,
        10.000032, 1);
}

SUITE(suite_pulse_time)
{
    uint16_t pulse_count, pulse_number;

    RUN_TEST(test_pulse_time_sanity);
    RUN_TEST(test_pulse_time_null_raster);
    RUN_TEST(test_pulse_time_null_pulse);
    RUN_TEST(test_pulse_time_count_0);
    RUN_TEST(test_pulse_time_number_0);
    RUN_TEST(test_pulse_time_number_high);

    for(pulse_count = 1; pulse_count <= 4; pulse_count++) {
        for(pulse_number = 1; pulse_number <= pulse_count; pulse_number++) {
            data_suite_pulse_time(pulse_number, pulse_count);
        }
    }
}

/*******************************************************************************
 * eaarlio_units_pulse_scan_angle
 *******************************************************************************
 */

TEST test_pulse_scan_angle_sanity()
{
    eaarlio_units_pulse_scan_angle(NULL);
    PASS();
}

TEST test_pulse_scan_angle_null()
{
    ASSERT_IN_RANGE_POWER(0., eaarlio_units_pulse_scan_angle(NULL), 0);
    PASS();
}

TEST test_pulse_scan_angle_data(int16_t counts, double expected, int tolerance)
{
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();
    pulse.scan_angle_counts = counts;
    ASSERT_IN_RANGE_POWER(
        expected, eaarlio_units_pulse_scan_angle(&pulse), tolerance);
    PASS();
}

SUITE(suite_pulse_scan_angle)
{
    RUN_TEST(test_pulse_scan_angle_sanity);
    RUN_TEST(test_pulse_scan_angle_null);

    RUN_TESTp(test_pulse_scan_angle_data, 0, 0., 0);
    RUN_TESTp(test_pulse_scan_angle_data, 100, 4.5, 1);
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
    RUN_SUITE(suite_edb_time);
    RUN_SUITE(suite_raster_time);
    RUN_SUITE(suite_pulse_time);
    RUN_SUITE(suite_pulse_scan_angle);

    GREATEST_MAIN_END();
}
