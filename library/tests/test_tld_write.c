#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_stream.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

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
 * Setup and teardown for test cases
 *******************************************************************************
 */

struct test_data {
    struct eaarlio_stream *stream;
    struct mock_stream *mock;
    size_t size;
    void *extra;
};

static void setup_data_cb(void *arg)
{
    struct test_data *data = (struct test_data *)arg;

    data->mock = mock_stream_new(data->size);
    assert(data->mock);

    data->stream = mock_stream_stream_new(data->mock);
    assert(data->stream);

    data->mock->sequential = 1;
    data->mock->no_read = 1;
}

static void teardown_data_cb(void *arg)
{
    struct test_data *data = (struct test_data *)arg;

    mock_stream_stream_destroy(data->stream);
    data->stream = NULL;

    mock_stream_destroy(data->mock);
    data->mock = NULL;
}

/*******************************************************************************
 * eaarlio_tld_write_raster
 *******************************************************************************
 */

/* Make sure we don't seg fault. */
TEST test_write_raster_sanity()
{
    eaarlio_tld_write_raster(NULL, NULL, NULL);
    PASS();
}

/* Can we handle a null stream? */
TEST test_write_raster_null_stream()
{
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_write_raster(NULL, &raster, NULL));
    PASS();
}

/* Can we handle a null raster? */
TEST test_write_raster_null_raster()
{
    struct eaarlio_stream stream;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_write_raster(&stream, NULL, NULL));
    PASS();
}

TEST test_write_raster_values(struct test_data *data)
{
    struct eaarlio_stream *stream = data->stream;
    int64_t offset;

    struct eaarlio_pulse pulses[] = {
        {
            .time_offset = 1249809,
            .rx_count = 1,
            .scan_angle_counts = 0,
            .range = 0,
            .thresh_tx = 0,
            .thresh_rx = 0,
            .bias_tx = 0,
            .bias_rx = { 0, 0, 0, 0 },
            .tx_len = 2,
            .rx_len = { 3, 0, 0, 0 },
            .tx = (unsigned char *)&(unsigned char[]){ '\x30', '\x31' },
            .rx = { (unsigned char *)&(unsigned char[]){
                        '\x40', '\x41', '\x42' },
                NULL, NULL, NULL },
        },
        {
            .time_offset = 2302497,
            .rx_count = 1,
            .scan_angle_counts = 0,
            .range = 0,
            .thresh_tx = 0,
            .thresh_rx = 0,
            .bias_tx = 0,
            .bias_rx = { 0, 0, 0, 0 },
            .tx_len = 2,
            .rx_len = { 2, 0, 0, 0 },
            .tx = (unsigned char *)&(unsigned char[]){ '\x51', '\x52' },
            .rx = { (unsigned char *)&(unsigned char[]){ '\x61', '\x62' }, NULL,
                NULL, NULL },
        },
    };

    struct eaarlio_raster raster = {
        .time_seconds = 67305985,
        .time_fraction = 0,
        .sequence_number = 0,
        .pulse_count = 2,
        .digitizer = 0,
        .pulse = (struct eaarlio_pulse *)&pulses,
    };

    unsigned char const exp[] = {
        /* record_header */
        /* raster length */
        '\x3F', '\x00', '\x00',
        /* type id, should be 5 */
        '\x05',
        /* raster_header */
        /* seconds */
        '\x01', '\x02', '\x03', '\x04',
        /* fractional seconds */
        '\x00', '\x00', '\x00', '\x00',
        /* sequence number */
        '\x00', '\x00', '\x00', '\x00',
        /* bitfield: pulse count and digitizer */
        '\x02', '\x00',

        /* pulse[0] */
        /* Offset time */
        '\x11', '\x12', '\x13',
        /* Number of waveforms */
        '\x01',
        /* Transmit bias */
        '\x00',
        /* Return biases */
        '\x00', '\x00', '\x00', '\x00',
        /* Scan angle counts */
        '\x00', '\x00',
        /* Bitfield: integer range, threshold flags */
        '\x00', '\x00',
        /* Data length */
        '\x08', '\x00',
        /* tx length */
        '\x02',
        /* tx waveform */
        '\x30', '\x31',
        /* rx[0] length */
        '\x03', '\x00',
        /* rx[0] waveform */
        '\x40', '\x41', '\x42',

        /* pulse[1] */
        /* Offset time */
        '\x21', '\x22', '\x23',
        /* Number of waveforms */
        '\x01',
        /* Transmit bias */
        '\x00',
        /* Return biases */
        '\x00', '\x00', '\x00', '\x00',
        /* Scan angle counts */
        '\x00', '\x00',
        /* Bitfield: integer range, threshold flags */
        '\x00', '\x00',
        /* Data length */
        '\x07', '\x00',
        /* tx length */
        '\x02',
        /* tx waveform */
        '\x51', '\x52',
        /* rx[0] length */
        '\x02', '\x00',
        /* rx[0] waveform */
        '\x61', '\x62',
    };

    ASSERT_EAARLIO_SUCCESS(stream->seek(stream, 0, SEEK_SET));
    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_write_raster(stream, &raster, NULL));
    ASSERT_EAARLIO_SUCCESS(stream->tell(stream, &offset));
    ASSERT_EQ_FMT(sizeof exp, offset, "%d");
    ASSERT_MEM_EQ(exp, data->mock->data, sizeof exp);
    PASS();
}

SUITE(suite_write_raster)
{
    struct test_data data;
    data.size = 200;

    RUN_TEST(test_write_raster_sanity);
    RUN_TEST(test_write_raster_null_stream);
    RUN_TEST(test_write_raster_null_raster);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    RUN_TESTp(test_write_raster_values, &data);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_write_raster);

    GREATEST_MAIN_END();
}
