#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_memory.h"
#include "mock_stream.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
/* Visual Studio complains about the unsigned char initialization */
#pragma warning(disable : 4245)
#endif

/*******************************************************************************
 * Setup and teardown for test cases
 *******************************************************************************
 */

struct test_mocks {
    size_t stream_size;
    struct eaarlio_stream *stream;
    struct mock_stream *stream_mock;

    int memory_size;
    struct eaarlio_memory *memory;
    struct mock_memory *memory_mock;
};

static void cb_mocks_setup(void *arg)
{
    struct test_mocks *data = (struct test_mocks *)arg;

    data->stream_mock = mock_stream_new(data->stream_size);
    assert(data->stream_mock);

    data->stream = mock_stream_stream_new(data->stream_mock);
    assert(data->stream);

    data->memory_mock = malloc(sizeof(struct mock_memory));
    assert(data->memory_mock);
    data->memory = malloc(sizeof(struct eaarlio_memory));
    assert(data->memory);

    mock_memory_new(data->memory, data->memory_mock, data->memory_size);

    unsigned char const buf[] = { /* record_header */
        /* raster length */
        '\x3F', '\x00', '\x00',
        /* type id, should be 5 */
        '\x05',
        /* raster_header */
        /* seconds */
        '\x01', '\x02', '\x03', '\x04',
        /* fractional seconds */
        '\x00', '\x00', '\x00', '\x00',
        /* raster number */
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

        /* extra garbage */
        '\xAA', '\xBB', '\xCC', '\xDD'
    };
    data->stream_mock->offset = 0;
    mock_stream_write(data->stream_mock, sizeof buf, buf);
    data->stream_mock->offset = 0;
}

static void cb_mocks_teardown(void *arg)
{
    struct test_mocks *data = (struct test_mocks *)arg;

    mock_stream_stream_destroy(data->stream);
    data->stream = NULL;

    mock_stream_destroy(data->stream_mock);
    data->stream_mock = NULL;

    mock_memory_destroy(data->memory);
    free(data->memory);
    data->memory = NULL;
    free(data->memory_mock);
    data->memory_mock = NULL;
}

/*******************************************************************************
 * eaarlio_tld_read_record
 *******************************************************************************
 */

/* Make sure we don't seg fault. */
TEST test_read_record_sanity()
{
    eaarlio_tld_read_record(NULL, NULL, NULL, NULL, 0, 0);
    PASS();
}

/* Can we handle a null stream? */
TEST test_read_record_null_stream()
{
    struct eaarlio_tld_header header;
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_read_record(NULL, &header, &raster, NULL, 0, 0));
    PASS();
}

/* Can we handle a null record header? */
TEST test_read_record_null_record_header()
{
    struct eaarlio_stream stream;
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_read_record(&stream, NULL, &raster, NULL, 0, 0));
    PASS();
}

/* Can we handle a null raster? */
TEST test_read_record_null_raster()
{
    struct eaarlio_stream stream;
    struct eaarlio_tld_header header;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_read_record(&stream, &header, NULL, NULL, 0, 0));
    PASS();
}

TEST test_record_values(char const *msg,
    struct eaarlio_stream *stream,
    struct eaarlio_memory *memory,
    int include_pulses,
    int include_waveforms)
{
    struct eaarlio_tld_header header;
    struct eaarlio_raster raster;
    int64_t offset;

    ASSERT_EAARLIO_SUCCESSm(msg, stream->seek(stream, 0, SEEK_SET));
    ASSERT_EAARLIO_SUCCESSm(
        msg, eaarlio_tld_read_record(stream, &header, &raster, memory,
                 include_pulses, include_waveforms));

    ASSERT_EAARLIO_SUCCESSm(msg, stream->tell(stream, &offset));
    ASSERT_EQ_FMTm(msg, 63, offset, "%d");

    ASSERT_EQ_FMTm(msg, 63, header.record_length, "%d");
    ASSERT_EQ_FMTm(msg, 5, header.record_type, "%d");

    ASSERT_EQ_FMTm(msg, 67305985, raster.time_seconds, "%d");
    ASSERT_EQ_FMTm(msg, 2, raster.pulse_count, "%d");

    if(!include_pulses) {
        ASSERTm(msg, !raster.pulse);
        PASS();
    }

    ASSERTm(msg, raster.pulse);

    ASSERT_EQ_FMTm(msg, 1249809, raster.pulse[0].time_offset, "%d");
    ASSERT_EQ_FMTm(msg, 1, raster.pulse[0].rx_count, "%d");

    ASSERT_EQ_FMTm(msg, 2302497, raster.pulse[1].time_offset, "%d");
    ASSERT_EQ_FMTm(msg, 1, raster.pulse[1].rx_count, "%d");

    ASSERTm(msg, !raster.pulse[0].rx[1]);
    ASSERTm(msg, !raster.pulse[0].rx[2]);
    ASSERTm(msg, !raster.pulse[0].rx[3]);

    ASSERTm(msg, !raster.pulse[1].rx[1]);
    ASSERTm(msg, !raster.pulse[1].rx[2]);
    ASSERTm(msg, !raster.pulse[1].rx[3]);

    if(!include_waveforms) {
        ASSERT_EQ_FMTm(msg, 0, raster.pulse[0].tx_len, "%d");
        ASSERT_EQ_FMTm(msg, 0, raster.pulse[0].rx_len[0], "%d");

        ASSERTm(msg, !raster.pulse[0].tx);
        ASSERTm(msg, !raster.pulse[0].rx[0]);

        ASSERT_EQ_FMTm(msg, 0, raster.pulse[1].tx_len, "%d");
        ASSERT_EQ_FMTm(msg, 0, raster.pulse[1].rx_len[0], "%d");

        ASSERTm(msg, !raster.pulse[1].tx);
        ASSERTm(msg, !raster.pulse[1].rx[0]);

        PASS();
    }

    ASSERTm(msg, raster.pulse[0].tx);
    ASSERTm(msg, raster.pulse[0].rx[0]);
    ASSERT_EQ_FMTm(msg, 2, raster.pulse[0].tx_len, "%d");
    ASSERT_EQ_FMTm(msg, 3, raster.pulse[0].rx_len[0], "%d");
    ASSERT_EQ_FMTm(msg, 0x30, raster.pulse[0].tx[0], "%02x");
    ASSERT_EQ_FMTm(msg, 0x31, raster.pulse[0].tx[1], "%02x");
    ASSERT_EQ_FMTm(msg, 0x40, raster.pulse[0].rx[0][0], "%02x");
    ASSERT_EQ_FMTm(msg, 0x41, raster.pulse[0].rx[0][1], "%02x");
    ASSERT_EQ_FMTm(msg, 0x42, raster.pulse[0].rx[0][2], "%02x");

    ASSERTm(msg, raster.pulse[1].tx);
    ASSERTm(msg, raster.pulse[1].rx[0]);
    ASSERT_EQ_FMTm(msg, 2, raster.pulse[1].tx_len, "%d");
    ASSERT_EQ_FMTm(msg, 2, raster.pulse[1].rx_len[0], "%d");
    ASSERT_EQ_FMTm(msg, 0x51, raster.pulse[1].tx[0], "%02x");
    ASSERT_EQ_FMTm(msg, 0x52, raster.pulse[1].tx[1], "%02x");
    ASSERT_EQ_FMTm(msg, 0x61, raster.pulse[1].rx[0][0], "%02x");
    ASSERT_EQ_FMTm(msg, 0x62, raster.pulse[1].rx[0][1], "%02x");

    PASS();
}

TEST test_invalid_stream(struct eaarlio_stream *stream,
    struct eaarlio_memory *memory)
{
    struct eaarlio_tld_header header;
    struct eaarlio_raster raster;
    struct eaarlio_stream bad = *stream;
    bad.read = NULL;

    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_INVALID,
        eaarlio_tld_read_record(&bad, &header, &raster, memory, 0, 0));
    PASS();
}

TEST test_invalid_memory(struct eaarlio_stream *stream,
    struct eaarlio_memory *memory)
{
    struct eaarlio_tld_header header;
    struct eaarlio_raster raster;
    struct eaarlio_memory bad = *memory;
    bad.free = NULL;

    ASSERT_EAARLIO_ERR(EAARLIO_MEMORY_INVALID,
        eaarlio_tld_read_record(stream, &header, &raster, &bad, 0, 0));
    PASS();
}

TEST test_oom(char const *msg,
    struct eaarlio_stream *stream,
    struct eaarlio_memory *memory)
{
    struct eaarlio_tld_header header;
    struct eaarlio_raster raster;

    ASSERT_EAARLIO_ERRm(msg, EAARLIO_MEMORY_ALLOC_FAIL,
        eaarlio_tld_read_record(stream, &header, &raster, memory, 1, 1));
    PASS();
}

TEST test_record_type_unknown(char const *msg,
    struct eaarlio_stream *stream,
    struct eaarlio_memory *memory,
    struct mock_stream *stream_mock,
    struct mock_memory *memory_mock,
    int include_pulses,
    int include_waveforms)
{
    struct eaarlio_tld_header header;
    struct eaarlio_raster raster = eaarlio_raster_empty();
    int64_t offset;

    stream_mock->data[3] = 42;

    ASSERT_EAARLIO_SUCCESSm(msg, stream->seek(stream, 0, SEEK_SET));
    ASSERT_EAARLIO_SUCCESSm(
        msg, eaarlio_tld_read_record(stream, &header, &raster, memory,
                 include_pulses, include_waveforms));

    ASSERT_EAARLIO_SUCCESSm(msg, stream->tell(stream, &offset));
    ASSERT_EQ_FMTm(msg, 63, offset, "%d");

    ASSERT_EQ_FMTm(msg, 63, header.record_length, "%d");
    ASSERT_EQ_FMTm(msg, 42, header.record_type, "%d");

    ASSERT_EQ_FMTm(msg, 0, raster.time_seconds, "%d");
    ASSERT_EQ_FMTm(msg, 0, raster.pulse_count, "%d");
    ASSERTm(msg, !raster.pulse);

    ASSERT_EQ_FMTm(msg, 0, mock_memory_count_in_use(memory_mock), "%d");

    PASS();
}

TEST test_corrupt(struct eaarlio_stream *stream,
    struct eaarlio_memory *memory,
    struct mock_stream *stream_mock,
    struct mock_memory *memory_mock)
{
    struct eaarlio_tld_header header;
    struct eaarlio_raster raster = eaarlio_raster_empty();

    stream_mock->data[0] = 1;

    ASSERT_EAARLIO_SUCCESS(stream->seek(stream, 0, SEEK_SET));
    ASSERT_EAARLIO_ERR(EAARLIO_CORRUPT,
        eaarlio_tld_read_record(stream, &header, &raster, memory, 1, 1));

    ASSERT_EQ_FMT(1, header.record_length, "%d");
    ASSERT_EQ_FMT(5, header.record_type, "%d");

    ASSERT_EQ_FMT(0, raster.time_seconds, "%d");
    ASSERT_EQ_FMT(0, raster.pulse_count, "%d");
    ASSERT(!raster.pulse);

    ASSERT_EQ_FMT(0, mock_memory_count_in_use(memory_mock), "%d");

    PASS();
}

SUITE(suite_read_record)
{
    struct test_mocks data;
    data.stream_size = 200;
    data.memory_size = 10;

    RUN_TEST(test_read_record_sanity);
    RUN_TEST(test_read_record_null_stream);
    RUN_TEST(test_read_record_null_record_header);
    RUN_TEST(test_read_record_null_raster);

    SET_SETUP(cb_mocks_setup, &data);
    SET_TEARDOWN(cb_mocks_teardown, &data);

    RUN_TESTp(test_record_values, "include_pulses=1, include_waveforms=1",
        data.stream, data.memory, 1, 1);
    RUN_TESTp(test_record_values, "include_pulses=1, include_waveforms=0",
        data.stream, data.memory, 1, 0);
    RUN_TESTp(test_record_values, "include_pulses=0, include_waveforms=0",
        data.stream, data.memory, 0, 0);

    RUN_TESTp(test_invalid_stream, data.stream, data.memory);
    RUN_TESTp(test_invalid_memory, data.stream, data.memory);

    data.memory_size = 0;
    RUN_TESTp(test_oom, "memory_size=0", data.stream, data.memory);
    data.memory_size = 1;
    RUN_TESTp(test_oom, "memory_size=1", data.stream, data.memory);
    data.memory_size = 2;
    RUN_TESTp(test_oom, "memory_size=2", data.stream, data.memory);
    data.memory_size = 3;
    RUN_TESTp(test_oom, "memory_size=3", data.stream, data.memory);
    data.memory_size = 4;
    RUN_TESTp(test_oom, "memory_size=4", data.stream, data.memory);
    data.memory_size = 5;
    RUN_TESTp(test_oom, "memory_size=5", data.stream, data.memory);
    data.memory_size = 10;

    RUN_TESTp(test_record_type_unknown, "include_pulses=1, include_waveforms=1",
        data.stream, data.memory, data.stream_mock, data.memory_mock, 1, 1);
    RUN_TESTp(test_record_type_unknown, "include_pulses=1, include_waveforms=0",
        data.stream, data.memory, data.stream_mock, data.memory_mock, 1, 0);
    RUN_TESTp(test_record_type_unknown, "include_pulses=0, include_waveforms=0",
        data.stream, data.memory, data.stream_mock, data.memory_mock, 0, 0);

    RUN_TESTp(test_corrupt, data.stream, data.memory, data.stream_mock,
        data.memory_mock);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_read_record);

    GREATEST_MAIN_END();
}
