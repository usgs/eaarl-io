#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_encode.h"
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
 * eaarlio_tld_encode_record_header
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_encode_record_header_sanity()
{
    unsigned char buf[EAARLIO_TLD_RECORD_HEADER_SIZE];
    struct eaarlio_tld_header const record_header = eaarlio_tld_header_empty();
    eaarlio_tld_encode_record_header(
        (unsigned char *)buf, EAARLIO_TLD_RECORD_HEADER_SIZE, &record_header);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_encode_record_header_null_buf()
{
    struct eaarlio_tld_header const record_header = eaarlio_tld_header_empty();
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_encode_record_header(NULL,
                          EAARLIO_TLD_RECORD_HEADER_SIZE, &record_header));
    PASS();
}

/* Can we handle a null record? */
TEST test_encode_record_header_null_record()
{
    unsigned char buf[EAARLIO_TLD_RECORD_HEADER_SIZE];
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_encode_record_header((unsigned char *)buf,
                          EAARLIO_TLD_RECORD_HEADER_SIZE, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_encode_record_header_buf_len0()
{
    unsigned char buf[1];
    struct eaarlio_tld_header const record_header = eaarlio_tld_header_empty();
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_encode_record_header(
                                  (unsigned char *)buf, 0, &record_header));
    PASS();
}

/* Can we handle a buffer size of 3?
 * 4 is the minimal size to write the struct */
TEST test_encode_record_header_buf_len3()
{
    unsigned char buf[3];
    struct eaarlio_tld_header const record_header = eaarlio_tld_header_empty();
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_encode_record_header(
                                  (unsigned char *)buf, 3, &record_header));
    PASS();
}

TEST test_encode_record_header_out_of_range(void *data)
{
    struct testcase_record_header *testcase =
        (struct testcase_record_header *)data;
    unsigned char got[EAARLIO_TLD_RECORD_HEADER_SIZE];
    ASSERT_EAARLIO_ERRm(testcase->name, EAARLIO_VALUE_OUT_OF_RANGE,
        eaarlio_tld_encode_record_header((unsigned char *)got,
            EAARLIO_TLD_RECORD_HEADER_SIZE, testcase->dec));
    PASS();
}

TEST test_encode_record_header_testcase(void *data)
{
    struct testcase_record_header *testcase =
        (struct testcase_record_header *)data;
    unsigned char got[EAARLIO_TLD_RECORD_HEADER_SIZE];
    assert(testcase->enc_len == EAARLIO_TLD_RECORD_HEADER_SIZE);
    ASSERT_EAARLIO_SUCCESSm(
        testcase->name, eaarlio_tld_encode_record_header((unsigned char *)got,
                            EAARLIO_TLD_RECORD_HEADER_SIZE, testcase->dec));
    ASSERT_MEM_EQm(
        testcase->name, testcase->enc, got, EAARLIO_TLD_RECORD_HEADER_SIZE);
    PASS();
}

SUITE(suite_encode_record_header)
{
    size_t i, count;
    struct testcase_record_header testcases_out_of_range[] = {
        {
            .name = "record_length_low",
            .dec = &(struct eaarlio_tld_header){.record_length = 16777216 },
            .enc = NULL,
        },
        {
            .name = "record_length_high",
            .dec = &(struct eaarlio_tld_header){.record_length = 4294967295 },
            .enc = NULL,
        },
    };

    RUN_TEST(test_encode_record_header_sanity);
    RUN_TEST(test_encode_record_header_null_buf);
    RUN_TEST(test_encode_record_header_null_record);
    RUN_TEST(test_encode_record_header_buf_len0);
    RUN_TEST(test_encode_record_header_buf_len3);

    count =
        sizeof testcases_out_of_range / sizeof(struct testcase_record_header);
    for(i = 0; i < count; i++) {
        RUN_TEST1(
            test_encode_record_header_out_of_range, &testcases_out_of_range[i]);
    }

    for(i = 0; i < testcases_record_header_len; i++) {
        RUN_TEST1(
            test_encode_record_header_testcase, &testcases_record_header[i]);
    }
}

/*******************************************************************************
 * eaarlio_tld_encode_raster_header
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_encode_raster_header_sanity()
{
    unsigned char buf[EAARLIO_TLD_RASTER_HEADER_SIZE];
    struct eaarlio_raster const raster = eaarlio_raster_empty();
    eaarlio_tld_encode_raster_header(
        (unsigned char *)buf, EAARLIO_TLD_RASTER_HEADER_SIZE, &raster);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_encode_raster_header_null_buf()
{
    struct eaarlio_raster const raster = eaarlio_raster_empty();
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_encode_raster_header(
                          NULL, EAARLIO_TLD_RASTER_HEADER_SIZE, &raster));
    PASS();
}

/* Can we handle a null raster? */
TEST test_encode_raster_header_null_raster()
{
    unsigned char buf[EAARLIO_TLD_RASTER_HEADER_SIZE];
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_encode_raster_header((unsigned char *)buf,
                          EAARLIO_TLD_RASTER_HEADER_SIZE, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_encode_raster_header_buf_len0()
{
    unsigned char buf[1];
    struct eaarlio_raster const raster = eaarlio_raster_empty();
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_raster_header((unsigned char *)buf, 0, &raster));
    PASS();
}

/* Can we handle a buffer size of 13?
 * 14 is the minimal size to write the struct */
TEST test_encode_raster_header_buf_len13()
{
    unsigned char buf[13];
    struct eaarlio_raster const raster = eaarlio_raster_empty();
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_raster_header((unsigned char *)buf, 13, &raster));
    PASS();
}

TEST test_encode_raster_header_out_of_range(void *data)
{
    struct testcase_raster_header *testcase =
        (struct testcase_raster_header *)data;
    unsigned char got[EAARLIO_TLD_RASTER_HEADER_SIZE];
    ASSERT_EAARLIO_ERRm(testcase->name, EAARLIO_VALUE_OUT_OF_RANGE,
        eaarlio_tld_encode_raster_header((unsigned char *)got,
            EAARLIO_TLD_RASTER_HEADER_SIZE, testcase->dec));
    PASS();
}

TEST test_encode_raster_header_testcase(void *data)
{
    struct testcase_raster_header *testcase =
        (struct testcase_raster_header *)data;
    unsigned char got[EAARLIO_TLD_RASTER_HEADER_SIZE];
    assert(testcase->enc_len == EAARLIO_TLD_RASTER_HEADER_SIZE);
    ASSERT_EAARLIO_SUCCESSm(
        testcase->name, eaarlio_tld_encode_raster_header((unsigned char *)got,
                            EAARLIO_TLD_RASTER_HEADER_SIZE, testcase->dec));
    ASSERT_MEM_EQm(
        testcase->name, testcase->enc, got, EAARLIO_TLD_RASTER_HEADER_SIZE);
    PASS();
}

SUITE(suite_encode_raster_header)
{
    size_t i, count;
    struct testcase_raster_header testcases_out_of_range[] = {
        {
            .name = "pulse_count_low",
            .dec = &(struct eaarlio_raster){.pulse_count = 32768 },
            .enc = NULL,
        },
        {
            .name = "pulse_count_high",
            .dec = &(struct eaarlio_raster){.pulse_count = 65535 },
            .enc = NULL,
        },
        {
            .name = "digitizer_low",
            .dec = &(struct eaarlio_raster){.digitizer = 2 },
            .enc = NULL,
        },
        {
            .name = "digitizer_high",
            .dec = &(struct eaarlio_raster){.digitizer = 255 },
            .enc = NULL,
        },
    };

    RUN_TEST(test_encode_raster_header_sanity);
    RUN_TEST(test_encode_raster_header_null_buf);
    RUN_TEST(test_encode_raster_header_null_raster);
    RUN_TEST(test_encode_raster_header_buf_len0);
    RUN_TEST(test_encode_raster_header_buf_len13);

    count =
        sizeof testcases_out_of_range / sizeof(struct testcase_raster_header);
    for(i = 0; i < count; i++) {
        RUN_TEST1(
            test_encode_raster_header_out_of_range, &testcases_out_of_range[i]);
    }

    for(i = 0; i < testcases_raster_header_len; i++) {
        RUN_TEST1(
            test_encode_raster_header_testcase, &testcases_raster_header[i]);
    }
}

/*******************************************************************************
 * eaarlio_tld_encode_pulse_header
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_encode_pulse_header_sanity()
{
    unsigned char buf[EAARLIO_TLD_PULSE_HEADER_SIZE];
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();
    eaarlio_tld_encode_pulse_header(
        (unsigned char *)buf, EAARLIO_TLD_PULSE_HEADER_SIZE, &pulse);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_encode_pulse_header_null_buf()
{
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_encode_pulse_header(
                          NULL, EAARLIO_TLD_PULSE_HEADER_SIZE, &pulse));
    PASS();
}

/* Can we handle a null pulse? */
TEST test_encode_pulse_header_null_pulse()
{
    unsigned char buf[EAARLIO_TLD_PULSE_HEADER_SIZE];
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_encode_pulse_header((unsigned char *)buf,
                          EAARLIO_TLD_PULSE_HEADER_SIZE, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_encode_pulse_header_buf_len0()
{
    unsigned char buf[1];
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_pulse_header((unsigned char *)buf, 0, &pulse));
    PASS();
}

/* Can we handle a buffer size of 12?
 * 13 is the minimal size to write the struct */
TEST test_encode_pulse_header_buf_len12()
{
    unsigned char buf[12];
    struct eaarlio_pulse pulse = eaarlio_pulse_empty();
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_pulse_header((unsigned char *)buf, 12, &pulse));
    PASS();
}

TEST test_encode_pulse_header_out_of_range(void *data)
{
    struct testcase_pulse_header *testcase =
        (struct testcase_pulse_header *)data;
    unsigned char got[EAARLIO_TLD_PULSE_HEADER_SIZE];
    ASSERT_EAARLIO_ERRm(testcase->name, EAARLIO_VALUE_OUT_OF_RANGE,
        eaarlio_tld_encode_pulse_header((unsigned char *)got,
            EAARLIO_TLD_PULSE_HEADER_SIZE, testcase->dec));
    PASS();
}

TEST test_encode_pulse_header_testcase(void *data)
{
    struct testcase_pulse_header *testcase =
        (struct testcase_pulse_header *)data;
    unsigned char got[EAARLIO_TLD_PULSE_HEADER_SIZE];
    assert(testcase->enc_len == EAARLIO_TLD_PULSE_HEADER_SIZE);
    ASSERT_EAARLIO_SUCCESSm(
        testcase->name, eaarlio_tld_encode_pulse_header((unsigned char *)got,
                            EAARLIO_TLD_PULSE_HEADER_SIZE, testcase->dec));
    ASSERT_MEM_EQm(
        testcase->name, testcase->enc, got, EAARLIO_TLD_PULSE_HEADER_SIZE);
    PASS();
}

SUITE(suite_encode_pulse_header)
{
    size_t i, count;
    struct testcase_pulse_header testcases_out_of_range[] = {
        {
            .name = "range_low",
            .dec = &(struct eaarlio_pulse){.range = 16384 },
            .enc = NULL,
        },
        {
            .name = "range_high",
            .dec = &(struct eaarlio_pulse){.range = 65535 },
            .enc = NULL,
        },
        {
            .name = "thresh_tx_low",
            .dec = &(struct eaarlio_pulse){.thresh_tx = 2 },
            .enc = NULL,
        },
        {
            .name = "thresh_tx_high",
            .dec = &(struct eaarlio_pulse){.thresh_tx = 255 },
            .enc = NULL,
        },
        {
            .name = "thresh_rx_low",
            .dec = &(struct eaarlio_pulse){.thresh_rx = 2 },
            .enc = NULL,
        },
        {
            .name = "thresh_rx_high",
            .dec = &(struct eaarlio_pulse){.thresh_rx = 255 },
            .enc = NULL,
        },
    };

    RUN_TEST(test_encode_pulse_header_sanity);
    RUN_TEST(test_encode_pulse_header_null_buf);
    RUN_TEST(test_encode_pulse_header_null_pulse);
    RUN_TEST(test_encode_pulse_header_buf_len0);
    RUN_TEST(test_encode_pulse_header_buf_len12);

    count =
        sizeof testcases_out_of_range / sizeof(struct testcase_pulse_header);
    for(i = 0; i < count; i++) {
        RUN_TEST1(
            test_encode_pulse_header_out_of_range, &testcases_out_of_range[i]);
    }

    for(i = 0; i < testcases_pulse_header_len; i++) {
        RUN_TEST1(
            test_encode_pulse_header_testcase, &testcases_pulse_header[i]);
    }
}

/*******************************************************************************
 * eaarlio_tld_encode_wf_data_length
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_encode_wf_data_length_sanity()
{
    unsigned char buf[2];
    eaarlio_tld_encode_wf_data_length((unsigned char *)buf, 2, 0);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_encode_wf_data_length_null_buf()
{
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_encode_wf_data_length(NULL, 2, 0));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_encode_wf_data_length_buf_len0()
{
    unsigned char buf[1];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_wf_data_length((unsigned char *)buf, 0, 0));
    PASS();
}

/* Can we handle a buffer size of 1?
 * 2 is the minimal length needed to parse a length */
TEST test_encode_wf_data_length_buf_len1()
{
    unsigned char buf[1];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_wf_data_length((unsigned char *)buf, 1, 0));
    PASS();
}

/* Can we encode the length correctly? */
TEST test_encode_wf_data_length_value()
{
    unsigned char buf[2];
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_tld_encode_wf_data_length((unsigned char *)buf, 2, 513));
    ASSERT_EQ_FMT(0x01U, buf[0], "%02x");
    ASSERT_EQ_FMT(0x02U, buf[1], "%02x");
    PASS();
}

SUITE(suite_encode_wf_data_length)
{
    RUN_TEST(test_encode_wf_data_length_sanity);
    RUN_TEST(test_encode_wf_data_length_null_buf);
    RUN_TEST(test_encode_wf_data_length_buf_len0);
    RUN_TEST(test_encode_wf_data_length_buf_len1);
    RUN_TEST(test_encode_wf_data_length_value);
}

/*******************************************************************************
 * eaarlio_tld_encode_tx_length
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_encode_tx_length_sanity()
{
    unsigned char buf[1];
    eaarlio_tld_encode_tx_length((unsigned char *)buf, 1, 0);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_encode_tx_length_null_buf()
{
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_tld_encode_tx_length(NULL, 1, 0));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_encode_tx_length_buf_len0()
{
    unsigned char buf[1];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_tx_length((unsigned char *)buf, 0, 0));
    PASS();
}

/* Can we encode the length correctly? */
TEST test_encode_tx_length_value()
{
    unsigned char buf[1];
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_tld_encode_tx_length((unsigned char *)buf, 1, 240));
    ASSERT_EQ_FMT(0xF0U, buf[0], "%02x");
    PASS();
}

SUITE(suite_encode_tx_length)
{
    RUN_TEST(test_encode_tx_length_sanity);
    RUN_TEST(test_encode_tx_length_null_buf);
    RUN_TEST(test_encode_tx_length_buf_len0);
    RUN_TEST(test_encode_tx_length_value);
}

/*******************************************************************************
 * eaarlio_tld_encode_rx_length
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_encode_rx_length_sanity()
{
    unsigned char buf[2];
    eaarlio_tld_encode_rx_length((unsigned char *)buf, 2, 0);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_encode_rx_length_null_buf()
{
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_tld_encode_rx_length(NULL, 2, 0));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_encode_rx_length_buf_len0()
{
    unsigned char buf[1];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_rx_length((unsigned char *)buf, 0, 0));
    PASS();
}

/* Can we handle a buffer size of 1?
 * 2 is the minimal length needed to parse a length */
TEST test_encode_rx_length_buf_len1()
{
    unsigned char buf[1];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_encode_rx_length((unsigned char *)buf, 1, 0));
    PASS();
}

/* Can we encode the length correctly? */
TEST test_encode_rx_length_value()
{
    unsigned char buf[2];
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_tld_encode_rx_length((unsigned char *)buf, 2, 513));
    ASSERT_EQ_FMT(0x01U, buf[0], "%02x");
    ASSERT_EQ_FMT(0x02U, buf[1], "%02x");
    PASS();
}

SUITE(suite_encode_rx_length)
{
    RUN_TEST(test_encode_rx_length_sanity);
    RUN_TEST(test_encode_rx_length_null_buf);
    RUN_TEST(test_encode_rx_length_buf_len0);
    RUN_TEST(test_encode_rx_length_buf_len1);
    RUN_TEST(test_encode_rx_length_value);
}

/*******************************************************************************
 * eaarlio_tld_encode_waveform
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_encode_waveform_sanity()
{
    unsigned char buf[5];
    unsigned char const wf[5] = { 0 };
    eaarlio_tld_encode_waveform(
        (unsigned char *)buf, 5, (unsigned char const *)wf, 5);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_encode_waveform_null_buf()
{
    unsigned char const wf[5] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_encode_waveform(NULL, 5, (unsigned char const *)wf, 5));
    PASS();
}

/* Can we handle a null waveform? */
TEST test_encode_waveform_null_wf()
{
    unsigned char buf[5];
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_encode_waveform((unsigned char *)buf, 5, NULL, 5));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_encode_waveform_buf_len0()
{
    unsigned char buf[1];
    unsigned char const wf[5] = { 0 };
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_encode_waveform((unsigned char *)buf,
                                  0, (unsigned char const *)wf, 5));
    PASS();
}

/* Can we handle a waveform size of 0? */
TEST test_encode_waveform_wf_len0()
{
    unsigned char buf[5];
    unsigned char const wf[1] = { 0 };
    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_encode_waveform(
        (unsigned char *)buf, 5, (unsigned char const *)wf, 0));
    PASS();
}

/* Can we handle a shorter buffer than waveform? */
TEST test_encode_waveform_buf_short()
{
    unsigned char buf[5];
    unsigned char wf[8];
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_encode_waveform((unsigned char *)buf,
                                  5, (unsigned char const *)wf, 8));
    PASS();
}

/* Can we write the waveform correctly?? */
TEST test_encode_waveform_value()
{
    unsigned char buf[5];
    unsigned char const wf[] = { '\x01', '\x02', '\x03', '\x04', '\x05' };
    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_encode_waveform(
        (unsigned char *)buf, 5, (unsigned char const *)wf, 5));
    ASSERT_EQ_FMT(1, buf[0], "%d");
    ASSERT_EQ_FMT(2, buf[1], "%d");
    ASSERT_EQ_FMT(3, buf[2], "%d");
    ASSERT_EQ_FMT(4, buf[3], "%d");
    ASSERT_EQ_FMT(5, buf[4], "%d");
    PASS();
}

SUITE(suite_encode_waveform)
{
    RUN_TEST(test_encode_waveform_sanity);
    RUN_TEST(test_encode_waveform_null_buf);
    RUN_TEST(test_encode_waveform_null_wf);
    RUN_TEST(test_encode_waveform_buf_len0);
    RUN_TEST(test_encode_waveform_wf_len0);
    RUN_TEST(test_encode_waveform_buf_short);
    RUN_TEST(test_encode_waveform_value);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_encode_record_header);
    RUN_SUITE(suite_encode_raster_header);
    RUN_SUITE(suite_encode_pulse_header);
    RUN_SUITE(suite_encode_wf_data_length);
    RUN_SUITE(suite_encode_tx_length);
    RUN_SUITE(suite_encode_rx_length);
    RUN_SUITE(suite_encode_waveform);

    GREATEST_MAIN_END();
}
