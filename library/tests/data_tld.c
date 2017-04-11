#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include "greatest.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
/* Visual Studio complains about the unsigned char initialization */
#pragma warning(disable : 4245)
#endif

/* A collection of encoded/decoded TLD data for use in testing.
 */

/* Utility functions for checking structs
 */
inline TEST check_record_header(char const *msg,
    struct eaarlio_tld_header *exp,
    struct eaarlio_tld_header *got)
{
    ASSERTm(msg, exp);
    ASSERTm(msg, got);
    ASSERT_EQ_FMTm(msg, exp->record_length, got->record_length, "%d");
    ASSERT_EQ_FMTm(msg, exp->record_type, got->record_type, "%d");
    PASS();
}

inline TEST check_pulse_header(char const *msg,
    struct eaarlio_pulse *exp,
    struct eaarlio_pulse *got)
{
    size_t i;
    size_t channels = EAARLIO_MAX_RX_COUNT;
    ASSERTm(msg, exp);
    ASSERTm(msg, got);
    ASSERT_EQ_FMTm(msg, exp->time_offset, got->time_offset, "%d");
    ASSERT_EQ_FMTm(msg, exp->rx_count, got->rx_count, "%d");
    ASSERT_EQ_FMTm(msg, exp->scan_angle_counts, got->scan_angle_counts, "%d");
    ASSERT_EQ_FMTm(msg, exp->range, got->range, "%d");
    ASSERT_EQ_FMTm(msg, exp->thresh_tx, got->thresh_tx, "%d");
    ASSERT_EQ_FMTm(msg, exp->thresh_rx, got->thresh_rx, "%d");
    ASSERT_EQ_FMTm(msg, exp->bias_tx, got->bias_tx, "%d");
    if(exp->rx_count < channels)
        channels = exp->rx_count;
    for(i = 0; i < channels; i++) {
        ASSERT_EQ_FMTm(msg, exp->bias_rx[i], got->bias_rx[i], "%d");
    }
    PASS();
}

inline TEST check_pulse(char const *msg,
    struct eaarlio_pulse *exp,
    struct eaarlio_pulse *got)
{
    size_t i;
    size_t channels = EAARLIO_MAX_RX_COUNT;
    ASSERTm(msg, exp);
    ASSERTm(msg, got);
    CHECK_CALL(check_pulse_header(msg, exp, got));
    if(exp->rx_count < channels)
        channels = exp->rx_count;
    ASSERT_EQ_FMTm(msg, exp->tx_len, got->tx_len, "%d");
    ASSERT_MEM_EQm(msg, exp->tx, got->tx, exp->tx_len);
    for(i = 0; i < channels; i++) {
        ASSERT_EQ_FMTm(msg, exp->rx_len[i], got->rx_len[i], "%d");
        ASSERT_MEM_EQm(msg, exp->rx[i], got->rx[i], exp->rx_len[i]);
    }
    PASS();
}

inline TEST check_raster_header(char const *msg,
    struct eaarlio_raster *exp,
    struct eaarlio_raster *got)
{
    ASSERTm(msg, exp);
    ASSERTm(msg, got);
    ASSERT_EQ_FMTm(msg, exp->time_seconds, got->time_seconds, "%d");
    ASSERT_EQ_FMTm(msg, exp->time_fraction, got->time_fraction, "%d");
    ASSERT_EQ_FMTm(msg, exp->sequence_number, got->sequence_number, "%d");
    ASSERT_EQ_FMTm(msg, exp->pulse_count, got->pulse_count, "%d");
    ASSERT_EQ_FMTm(msg, exp->digitizer, got->digitizer, "%d");
    PASS();
}

inline TEST check_raster(char const *msg,
    struct eaarlio_raster *exp,
    struct eaarlio_raster *got)
{
    uint_fast16_t i;
    ASSERTm(msg, exp);
    ASSERTm(msg, got);
    CHECK_CALL(check_raster_header(msg, exp, got));
    for(i = 0; i < exp->pulse_count; i++) {
        CHECK_CALL(check_pulse(msg, &exp->pulse[i], &got->pulse[i]));
    }
    PASS();
}

/* *** record_header *** */

struct eaarlio_tld_header dec_record_header_unique = {.record_length = 197121,
    .record_type = 4 };

unsigned char const enc_record_header_unique[] = {
    /* raster length */
    '\x01', '\x02', '\x03',
    /* type id, should be 5 */
    '\x04'
};

struct eaarlio_tld_header dec_record_header_len_low = {.record_length = 0,
    .record_type = 5 };

unsigned char const enc_record_header_len_low[] = {
    /* raster length */
    '\x00', '\x00', '\x00',
    /* type id, should be 5 */
    '\x05'
};

struct eaarlio_tld_header dec_record_header_len_high = {.record_length =
                                                            16777215,
    .record_type = 5 };

unsigned char const enc_record_header_len_high[] = {
    /* raster length */
    '\xff', '\xff', '\xff',
    /* type id, should be 5 */
    '\x05'
};

struct testcase_record_header {
    char const *name;
    struct eaarlio_tld_header *dec;
    unsigned char const *enc;
    size_t enc_len;
};

struct testcase_record_header testcases_record_header[] = {
    {.name = "unique",
        .dec = &dec_record_header_unique,
        .enc = (unsigned char *)enc_record_header_unique,
        .enc_len = sizeof enc_record_header_unique },
    {.name = "len_low",
        .dec = &dec_record_header_len_low,
        .enc = (unsigned char *)enc_record_header_len_low,
        .enc_len = sizeof enc_record_header_len_low },
    {.name = "len_high",
        .dec = &dec_record_header_len_high,
        .enc = (unsigned char *)enc_record_header_len_high,
        .enc_len = sizeof enc_record_header_len_high },
};

size_t testcases_record_header_len =
    sizeof testcases_record_header / sizeof(struct testcase_record_header);

/* *** raster_header *** */

struct eaarlio_raster dec_raster_header_unique = {
    .time_seconds = 134678020,
    .time_fraction = 202050057,
    .sequence_number = 269422093,
    .pulse_count = 4625,
    .digitizer = 0,
    .pulse = NULL,
};

unsigned char const enc_raster_header_unique[] = {
    /* seconds */
    '\x04', '\x06', '\x07', '\x08',
    /* fractional seconds */
    '\x09', '\x0A', '\x0B', '\x0C',
    /* sequence number */
    '\x0D', '\x0E', '\x0F', '\x10',
    /* bitfield: pulse count and digitizer */
    '\x11', '\x12'
};

struct eaarlio_raster dec_raster_header_digitizer = {
    .time_seconds = 0,
    .time_fraction = 0,
    .sequence_number = 0,
    .pulse_count = 0,
    .digitizer = 1,
    .pulse = NULL,
};

unsigned char const enc_raster_header_digitizer[] = {
    /* seconds */
    '\x00', '\x00', '\x00', '\x00',
    /* fractional seconds */
    '\x00', '\x00', '\x00', '\x00',
    /* sequence number */
    '\x00', '\x00', '\x00', '\x00',
    /* bitfield: pulse count and digitizer */
    '\x00', '\x80'
};

struct eaarlio_raster dec_raster_header_pulse_high = {
    .time_seconds = 0,
    .time_fraction = 0,
    .sequence_number = 0,
    .pulse_count = 32767,
    .digitizer = 0,
    .pulse = NULL,
};

unsigned char const enc_raster_header_pulse_high[] = {
    /* seconds */
    '\x00', '\x00', '\x00', '\x00',
    /* fractional seconds */
    '\x00', '\x00', '\x00', '\x00',
    /* sequence number */
    '\x00', '\x00', '\x00', '\x00',
    /* bitfield: pulse count and digitizer */
    '\xff', '\x7f'
};

struct testcase_raster_header {
    char const *name;
    struct eaarlio_raster *dec;
    unsigned char const *enc;
    size_t enc_len;
};

struct testcase_raster_header testcases_raster_header[] = {
    {.name = "unique",
        .dec = &dec_raster_header_unique,
        .enc = (unsigned char *)enc_raster_header_unique,
        .enc_len = sizeof enc_raster_header_unique },
    {.name = "digitizer",
        .dec = &dec_raster_header_digitizer,
        .enc = (unsigned char *)enc_raster_header_digitizer,
        .enc_len = sizeof enc_raster_header_digitizer },
    {.name = "pulse_high",
        .dec = &dec_raster_header_pulse_high,
        .enc = (unsigned char *)enc_raster_header_pulse_high,
        .enc_len = sizeof enc_raster_header_pulse_high },
};

size_t testcases_raster_header_len =
    sizeof testcases_raster_header / sizeof(struct testcase_raster_header);

/* *** pulse_header *** */

struct eaarlio_pulse dec_pulse_header_unique = {
    .time_offset = 197121,
    .rx_count = 4,
    .bias_tx = 5,
    .bias_rx = { 6, 7, 8, 9 },
    .scan_angle_counts = 2826,
    .range = 3340,
    .thresh_tx = 0,
    .thresh_rx = 0,
    .tx_len = 0,
    .rx_len = { 0, 0, 0, 0 },
    .tx = NULL,
    .rx = { NULL, NULL, NULL, NULL },
};

unsigned char const enc_pulse_header_unique[] = {
    /* Offset time */
    '\x01', '\x02', '\x03',
    /* Number of waveforms */
    '\x04',
    /* Transmit bias */
    '\x05',
    /* Return biases */
    '\x06', '\x07', '\x08', '\x09',
    /* Scan angle counts */
    '\x0A', '\x0B',
    /* Bitfield: integer range, threshold flags */
    '\x0C', '\x0D'
};

struct eaarlio_pulse dec_pulse_header_thresh_tx = {
    .time_offset = 0,
    .rx_count = 4,
    .bias_tx = 0,
    .bias_rx = { 0, 0, 0, 0 },
    .scan_angle_counts = 0,
    .range = 0,
    .thresh_tx = 1,
    .thresh_rx = 0,
    .tx_len = 0,
    .rx_len = { 0, 0, 0, 0 },
    .tx = NULL,
    .rx = { NULL, NULL, NULL, NULL },
};

unsigned char const enc_pulse_header_thresh_tx[] = {
    /* Offset time */
    '\x00', '\x00', '\x00',
    /* Number of waveforms */
    '\x04',
    /* Transmit bias */
    '\x00',
    /* Return biases */
    '\x00', '\x00', '\x00', '\x00',
    /* Scan angle counts */
    '\x00', '\x00',
    /* Bitfield: integer range, threshold flags */
    '\x00', '\x40'
};

struct eaarlio_pulse dec_pulse_header_thresh_rx = {
    .time_offset = 0,
    .rx_count = 4,
    .bias_tx = 0,
    .bias_rx = { 0, 0, 0, 0 },
    .scan_angle_counts = 0,
    .range = 0,
    .thresh_tx = 0,
    .thresh_rx = 1,
    .tx_len = 0,
    .rx_len = { 0, 0, 0, 0 },
    .tx = NULL,
    .rx = { NULL, NULL, NULL, NULL },
};

unsigned char const enc_pulse_header_thresh_rx[] = {
    /* Offset time */
    '\x00', '\x00', '\x00',
    /* Number of waveforms */
    '\x04',
    /* Transmit bias */
    '\x00',
    /* Return biases */
    '\x00', '\x00', '\x00', '\x00',
    /* Scan angle counts */
    '\x00', '\x00',
    /* Bitfield: integer range, threshold flags */
    '\x00', '\x80'
};

struct eaarlio_pulse dec_pulse_header_wf0 = {
    .time_offset = 0,
    .rx_count = 0,
    .bias_tx = 0,
    .bias_rx = { 0, 0, 0, 0 },
    .scan_angle_counts = 0,
    .range = 0,
    .thresh_tx = 0,
    .thresh_rx = 0,
    .tx_len = 0,
    .rx_len = { 0, 0, 0, 0 },
    .tx = NULL,
    .rx = { NULL, NULL, NULL, NULL },
};

unsigned char const enc_pulse_header_wf0[] = {
    /* Offset time */
    '\x00', '\x00', '\x00',
    /* Number of waveforms */
    '\x00',
    /* Transmit bias */
    '\x00',
    /* Return biases */
    '\x00', '\x00', '\x00', '\x00',
    /* Scan angle counts */
    '\x00', '\x00',
    /* Bitfield: integer range, threshold flags */
    '\x00', '\x00'
};

struct eaarlio_pulse dec_pulse_header_wf255 = {
    .time_offset = 0,
    .rx_count = 255,
    .bias_tx = 0,
    .bias_rx = { 0, 0, 0, 0 },
    .scan_angle_counts = 0,
    .range = 0,
    .thresh_tx = 0,
    .thresh_rx = 0,
    .tx_len = 0,
    .rx_len = { 0, 0, 0, 0 },
    .tx = NULL,
    .rx = { NULL, NULL, NULL, NULL },
};

unsigned char const enc_pulse_header_wf255[] = {
    /* Offset time */
    '\x00', '\x00', '\x00',
    /* Number of waveforms */
    '\xff',
    /* Transmit bias */
    '\x00',
    /* Return biases */
    '\x00', '\x00', '\x00', '\x00',
    /* Scan angle counts */
    '\x00', '\x00',
    /* Bitfield: integer range, threshold flags */
    '\x00', '\x00'
};

struct eaarlio_pulse dec_pulse_header_range_high = {
    .time_offset = 0,
    .rx_count = 0,
    .bias_tx = 0,
    .bias_rx = { 0, 0, 0, 0 },
    .scan_angle_counts = 0,
    .range = 16383,
    .thresh_tx = 0,
    .thresh_rx = 0,
    .tx_len = 0,
    .rx_len = { 0, 0, 0, 0 },
    .tx = NULL,
    .rx = { NULL, NULL, NULL, NULL },
};

unsigned char const enc_pulse_header_range_high[] = {
    /* Offset time */
    '\x00', '\x00', '\x00',
    /* Number of waveforms */
    '\x00',
    /* Transmit bias */
    '\x00',
    /* Return biases */
    '\x00', '\x00', '\x00', '\x00',
    /* Scan angle counts */
    '\x00', '\x00',
    /* Bitfield: integer range, threshold flags */
    '\xff', '\x3f'
};

struct testcase_pulse_header {
    char const *name;
    struct eaarlio_pulse *dec;
    unsigned char const *enc;
    size_t enc_len;
};

struct testcase_pulse_header testcases_pulse_header[] = {
    {.name = "unique",
        .dec = &dec_pulse_header_unique,
        .enc = (unsigned char *)enc_pulse_header_unique,
        .enc_len = sizeof enc_pulse_header_unique },
    {.name = "thresh_tx",
        .dec = &dec_pulse_header_thresh_tx,
        .enc = (unsigned char *)enc_pulse_header_thresh_tx,
        .enc_len = sizeof enc_pulse_header_thresh_tx },
    {.name = "thresh_rx",
        .dec = &dec_pulse_header_thresh_rx,
        .enc = (unsigned char *)enc_pulse_header_thresh_rx,
        .enc_len = sizeof enc_pulse_header_thresh_rx },
    {.name = "wf0",
        .dec = &dec_pulse_header_wf0,
        .enc = (unsigned char *)enc_pulse_header_wf0,
        .enc_len = sizeof enc_pulse_header_wf0 },
    {.name = "wf255",
        .dec = &dec_pulse_header_wf255,
        .enc = (unsigned char *)enc_pulse_header_wf255,
        .enc_len = sizeof enc_pulse_header_wf255 },
    {.name = "range_high",
        .dec = &dec_pulse_header_range_high,
        .enc = (unsigned char *)enc_pulse_header_range_high,
        .enc_len = sizeof enc_pulse_header_range_high },
};

size_t testcases_pulse_header_len =
    sizeof testcases_pulse_header / sizeof(struct testcase_pulse_header);
