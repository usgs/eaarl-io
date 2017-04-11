#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/flight.h"
#include "eaarlio/stream.h"
#include "eaarlio/tld_opener.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_memory.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define EDB_FILE (DATADIR "/flight.idx")

TEST test_sanity()
{
    eaarlio_file_flight(NULL, NULL, NULL, NULL);
    PASS();
}

TEST test_null_flight()
{
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_file_flight(NULL, "edb_file", "tld_path", NULL));
    PASS();
}

TEST test_null_edb()
{
    struct eaarlio_flight flight;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_file_flight(&flight, NULL, "tld_path", NULL));
    PASS();
}

TEST test_null_tld()
{
    struct eaarlio_flight flight;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_file_flight(&flight, "edb_file", NULL, NULL));
    PASS();
}

SUITE(suite_basic)
{
    RUN_TEST(test_sanity);
    RUN_TEST(test_null_flight);
    RUN_TEST(test_null_edb);
    RUN_TEST(test_null_tld);
}

TEST test_open(struct eaarlio_memory *memory)
{
    struct eaarlio_flight flight;
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_file_flight(&flight, EDB_FILE, DATADIR, memory));
    PASS();
}

TEST test_open_free(struct eaarlio_memory *memory, struct mock_memory *mock)
{
    struct eaarlio_flight flight;
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_file_flight(&flight, EDB_FILE, DATADIR, memory));
    ASSERT_EAARLIO_SUCCESS(eaarlio_flight_free(&flight));
    ASSERT_EQ_FMT(0, mock_memory_count_in_use(mock), "%d");
    PASS();
}

SUITE(suite_memory)
{
    struct mock_memory mock;
    struct eaarlio_memory memory;

    mock_memory_new(&memory, &mock, 100);
    RUN_TESTp(test_open, &memory);

    mock_memory_reset(&mock, 100);
    RUN_TESTp(test_open_free, &memory, &mock);

    mock_memory_destroy(&memory);
}

TEST test_raster_read(char const *msg,
    struct eaarlio_memory *memory,
    int32_t time_offset,
    unsigned int raster_number,
    int include_pulses,
    int include_waveforms)
{
    struct eaarlio_flight flight;
    int i, j;
    struct eaarlio_raster raster;
    int32_t time_offset_got;

    ASSERT_EAARLIO_SUCCESSm(
        msg, eaarlio_file_flight(&flight, EDB_FILE, DATADIR, memory));

    flight.edb.records[raster_number - 1].time_seconds += time_offset;

    ASSERT_EAARLIO_SUCCESSm(
        msg, eaarlio_flight_read_raster(&flight, &raster, &time_offset_got,
                 raster_number, include_pulses, include_waveforms));

    ASSERT_EQ_FMTm(msg, time_offset, time_offset_got, "%d");
    ASSERT_EQ_FMTm(msg, raster_number, raster.sequence_number, "%d");
    ASSERT_EQ_FMTm(msg, 1000000000 + raster_number, raster.time_seconds, "%d");
    ASSERT_EQ_FMTm(msg, 119, raster.pulse_count, "%d");

    if(include_pulses) {
        ASSERTm(msg, raster.pulse);
        for(i = 0; i < raster.pulse_count; i++) {
            ASSERT_EQ_FMTm(msg, raster_number * 1000 + i + 1,
                raster.pulse[i].time_offset, "%d");
            ASSERT_EQ_FMTm(msg, 4, raster.pulse[i].rx_count, "%d");

            if(include_waveforms) {
                ASSERTm(msg, raster.pulse[i].tx);
                ASSERT_EQ_FMTm(msg, 16, raster.pulse[i].tx_len, "%d");
                ASSERT_EQ_FMTm(msg, 255, raster.pulse[i].tx[0], "%d");
                for(j = 0; j < 4; j++) {
                    ASSERTm(msg, raster.pulse[i].rx[j]);
                    ASSERT_EQ_FMTm(msg, 32, raster.pulse[i].rx_len[j], "%d");
                    ASSERT_EQ_FMTm(msg, 255, raster.pulse[i].rx[j][0], "%d");
                }
            } else {
                ASSERT_FALSEm(msg, raster.pulse[i].tx);
                for(j = 0; j < 4; j++) {
                    ASSERT_FALSEm(msg, raster.pulse[i].rx[j]);
                }
            }
        }
    } else {
        ASSERT_FALSEm(msg, raster.pulse);
    }

    ASSERT_EAARLIO_SUCCESSm(msg, eaarlio_flight_free(&flight));
    PASS();
}

SUITE(suite_raster)
{
    struct mock_memory mock;
    struct eaarlio_memory memory;

    mock_memory_new(&memory, &mock, 100);
    RUN_TESTp(test_raster_read, "first raster, no pulses", &memory, 0, 1, 0, 0);

    mock_memory_reset(&mock, 100);
    RUN_TESTp(test_raster_read, "fifth raster, pulses, no waveforms", &memory,
        0, 5, 1, 0);

    mock_memory_reset(&mock, 1000);
    RUN_TESTp(test_raster_read, "tenth raster, pulses + waveforms", &memory, 0,
        10, 1, 1);

    mock_memory_reset(&mock, 100);
    RUN_TESTp(
        test_raster_read, "second raster, offset = -3", &memory, -3, 2, 0, 0);

    mock_memory_reset(&mock, 100);
    RUN_TESTp(
        test_raster_read, "third raster, offset = 2", &memory, 2, 3, 0, 0);

    mock_memory_destroy(&memory);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_basic);
    RUN_SUITE(suite_memory);
    RUN_SUITE(suite_raster);

    GREATEST_MAIN_END();
}
