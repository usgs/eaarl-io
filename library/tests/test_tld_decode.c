#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_decode.h"
#include "greatest.h"
#include "assert_error.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "data_tld.c"

/*******************************************************************************
 * eaarlio_tld_decode_record_header
 *
 * eaarlio_error eaarlio_tld_decode_record_header(unsigned char const *buffer,
 *     uint32_t buffer_len, struct eaarlio_tld_header *record_header);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_decode_record_header_sanity()
{
    unsigned char buf[EAARLIO_TLD_RECORD_HEADER_SIZE];
    struct eaarlio_tld_header record_header;
    eaarlio_tld_decode_record_header((unsigned char const *)buf,
        EAARLIO_TLD_RECORD_HEADER_SIZE, &record_header);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_decode_record_header_null_buf()
{
    struct eaarlio_tld_header record_header;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_decode_record_header(NULL,
                          EAARLIO_TLD_RECORD_HEADER_SIZE, &record_header));
    PASS();
}

/* Can we handle a null record? */
TEST test_decode_record_header_null_record()
{
    unsigned char const buf[EAARLIO_TLD_RECORD_HEADER_SIZE] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_record_header(
            (unsigned char const *)buf, EAARLIO_TLD_RECORD_HEADER_SIZE, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_decode_record_header_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    struct eaarlio_tld_header record_header;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_record_header(
            (unsigned char const *)buf, 0, &record_header));
    PASS();
}

/* Can we handle a buffer size of 3?
 * 4 is the minimal size to read the struct */
TEST test_decode_record_header_buf_len3()
{
    unsigned char const buf[3] = { 0 };
    struct eaarlio_tld_header record_header;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_record_header(
            (unsigned char const *)buf, 3, &record_header));
    PASS();
}

TEST test_decode_record_header_testcase(void *data)
{
    struct testcase_record_header *testcase =
        (struct testcase_record_header *)data;
    struct eaarlio_tld_header got;
    ASSERT_EAARLIO_SUCCESSm(
        testcase->name, eaarlio_tld_decode_record_header(
                            testcase->enc, (uint32_t)testcase->enc_len, &got));
    CHECK_CALL(check_record_header("unique", testcase->dec, &got));
    PASS();
}

SUITE(suite_decode_record_header)
{
    RUN_TEST(test_decode_record_header_sanity);
    RUN_TEST(test_decode_record_header_null_buf);
    RUN_TEST(test_decode_record_header_null_record);
    RUN_TEST(test_decode_record_header_buf_len0);
    RUN_TEST(test_decode_record_header_buf_len3);

    size_t i;
    for(i = 0; i < testcases_record_header_len; i++) {
        RUN_TEST1(
            test_decode_record_header_testcase, &testcases_record_header[i]);
    }
}

/*******************************************************************************
 * eaarlio_tld_decode_raster_header
 *
 * eaarlio_error eaarlio_tld_decode_raster_header(unsigned char const *buffer,
 *      uint32_t buffer_len, struct eaarlio_raster *raster);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_decode_raster_header_sanity()
{
    unsigned char const buf[EAARLIO_TLD_RASTER_HEADER_SIZE] = { 0 };
    struct eaarlio_raster raster;
    eaarlio_tld_decode_raster_header(
        (unsigned char const *)buf, EAARLIO_TLD_RASTER_HEADER_SIZE, &raster);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_decode_raster_header_null_buf()
{
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_decode_raster_header(
                          NULL, EAARLIO_TLD_RASTER_HEADER_SIZE, &raster));
    PASS();
}

/* Can we handle a null raster? */
TEST test_decode_raster_header_null_raster()
{
    unsigned char const buf[EAARLIO_TLD_RASTER_HEADER_SIZE] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_raster_header(
            (unsigned char const *)buf, EAARLIO_TLD_RASTER_HEADER_SIZE, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_decode_raster_header_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_decode_raster_header(
                                  (unsigned char const *)buf, 0, &raster));
    PASS();
}

/* Can we handle a buffer size of 13?
 * 14 is the minimal size to read the struct */
TEST test_decode_raster_header_buf_len13()
{
    unsigned char const buf[13] = { 0 };
    struct eaarlio_raster raster;
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_decode_raster_header(
                                  (unsigned char const *)buf, 13, &raster));
    PASS();
}

TEST test_decode_raster_header_testcase(void *data)
{
    struct testcase_raster_header *testcase =
        (struct testcase_raster_header *)data;
    struct eaarlio_raster got;
    ASSERT_EAARLIO_SUCCESSm(
        testcase->name, eaarlio_tld_decode_raster_header(
                            testcase->enc, (uint32_t)testcase->enc_len, &got));
    CHECK_CALL(check_raster_header(testcase->name, testcase->dec, &got));
    PASS();
}

SUITE(suite_decode_raster_header)
{
    RUN_TEST(test_decode_raster_header_sanity);
    RUN_TEST(test_decode_raster_header_null_buf);
    RUN_TEST(test_decode_raster_header_null_raster);
    RUN_TEST(test_decode_raster_header_buf_len0);
    RUN_TEST(test_decode_raster_header_buf_len13);

    size_t i;
    for(i = 0; i < testcases_raster_header_len; i++) {
        RUN_TEST1(
            test_decode_raster_header_testcase, &testcases_raster_header[i]);
    }
}

/*******************************************************************************
 * eaarlio_tld_decode_pulse_header
 *
 * eaarlio_error eaarlio_tld_decode_pulse_header(unsigned char const *buffer,
 *      uint32_t buffer_len, struct eaarlio_pulse *pulse);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_decode_pulse_header_sanity()
{
    unsigned char const buf[EAARLIO_TLD_PULSE_HEADER_SIZE] = { 0 };
    struct eaarlio_pulse pulse;
    eaarlio_tld_decode_pulse_header(
        (unsigned char const *)buf, EAARLIO_TLD_PULSE_HEADER_SIZE, &pulse);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_decode_pulse_header_null_buf()
{
    struct eaarlio_pulse pulse;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_decode_pulse_header(
                          NULL, EAARLIO_TLD_PULSE_HEADER_SIZE, &pulse));
    PASS();
}

/* Can we handle a null pulse? */
TEST test_decode_pulse_header_null_pulse()
{
    unsigned char const buf[EAARLIO_TLD_PULSE_HEADER_SIZE] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_pulse_header(
            (unsigned char const *)buf, EAARLIO_TLD_PULSE_HEADER_SIZE, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_decode_pulse_header_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    struct eaarlio_pulse pulse;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_pulse_header((unsigned char const *)buf, 0, &pulse));
    PASS();
}

/* Can we handle a buffer size of 12?
 * 13 is the minimal size to read the struct */
TEST test_decode_pulse_header_buf_len12()
{
    unsigned char const buf[12] = { 0 };
    struct eaarlio_pulse pulse;
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_decode_pulse_header(
                                  (unsigned char const *)buf, 12, &pulse));
    PASS();
}

TEST test_decode_pulse_header_testcase(void *data)
{
    struct testcase_pulse_header *testcase =
        (struct testcase_pulse_header *)data;
    struct eaarlio_pulse got;
    ASSERT_EAARLIO_SUCCESSm(
        testcase->name, eaarlio_tld_decode_pulse_header(
                            testcase->enc, (uint32_t)testcase->enc_len, &got));
    CHECK_CALL(check_pulse_header(testcase->name, testcase->dec, &got));
    PASS();
}

SUITE(suite_decode_pulse_header)
{
    RUN_TEST(test_decode_pulse_header_sanity);
    RUN_TEST(test_decode_pulse_header_null_buf);
    RUN_TEST(test_decode_pulse_header_null_pulse);
    RUN_TEST(test_decode_pulse_header_buf_len0);
    RUN_TEST(test_decode_pulse_header_buf_len12);

    size_t i;
    for(i = 0; i < testcases_pulse_header_len; i++) {
        RUN_TEST1(
            test_decode_pulse_header_testcase, &testcases_pulse_header[i]);
    }
}

/*******************************************************************************
 * eaarlio_tld_decode_wf_data_length
 *
 * eaarlio_error eaarlio_tld_decode_wf_data_length(unsigned char const *buffer,
 *     uint32_t buffer_len, uint16_t *length);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_decode_wf_data_length_sanity()
{
    unsigned char const buf[2] = { 0 };
    uint16_t length;
    eaarlio_tld_decode_wf_data_length((unsigned char const *)buf, 2, &length);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_decode_wf_data_length_null_buf()
{
    uint16_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_decode_wf_data_length(NULL, 2, &length));
    PASS();
}

/* Can we handle a null length? */
TEST test_decode_wf_data_length_null_length()
{
    unsigned char const buf[2] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_wf_data_length((unsigned char const *)buf, 2, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_decode_wf_data_length_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    uint16_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_decode_wf_data_length(
                                  (unsigned char const *)buf, 0, &length));
    PASS();
}

/* Can we handle a buffer size of 1?
 * 2 is the minimal length needed to parse a length */
TEST test_decode_wf_data_length_buf_len1()
{
    unsigned char const buf[1] = { 0 };
    uint16_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_tld_decode_wf_data_length(
                                  (unsigned char const *)buf, 1, &length));
    PASS();
}

/* Can we parse the length correctly? */
TEST test_decode_wf_data_length_value()
{
    unsigned char const buf[] = { '\x01', '\x02' };
    uint16_t length;
    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_decode_wf_data_length(
        (unsigned char const *)buf, 2, &length));
    ASSERT_EQ_FMT(513, length, "%d");
    PASS();
}

SUITE(suite_decode_wf_data_length)
{
    RUN_TEST(test_decode_wf_data_length_sanity);
    RUN_TEST(test_decode_wf_data_length_null_buf);
    RUN_TEST(test_decode_wf_data_length_null_length);
    RUN_TEST(test_decode_wf_data_length_buf_len0);
    RUN_TEST(test_decode_wf_data_length_buf_len1);
    RUN_TEST(test_decode_wf_data_length_value);
}

/*******************************************************************************
 * eaarlio_tld_decode_tx_length
 *
 * eaarlio_error eaarlio_tld_decode_tx_length(unsigned char const *buffer,
 *     uint32_t buffer_len, uint16_t *length);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_decode_tx_length_sanity()
{
    unsigned char const buf[1] = { 0 };
    uint8_t length;
    eaarlio_tld_decode_tx_length((unsigned char const *)buf, 1, &length);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_decode_tx_length_null_buf()
{
    uint8_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_decode_tx_length(NULL, 1, &length));
    PASS();
}

/* Can we handle a null length? */
TEST test_decode_tx_length_null_length()
{
    unsigned char const buf[1] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_tx_length((unsigned char const *)buf, 1, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_decode_tx_length_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    uint8_t length;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_tx_length((unsigned char const *)buf, 0, &length));
    PASS();
}

/* Can we parse the length correctly? */
TEST test_decode_tx_length_value()
{
    unsigned char const buf[] = { '\xF0' };
    uint8_t length;
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_tld_decode_tx_length((unsigned char const *)buf, 1, &length));
    ASSERT_EQ_FMT(240, length, "%d");
    PASS();
}

SUITE(suite_decode_tx_length)
{
    RUN_TEST(test_decode_tx_length_sanity);
    RUN_TEST(test_decode_tx_length_null_buf);
    RUN_TEST(test_decode_tx_length_null_length);
    RUN_TEST(test_decode_tx_length_buf_len0);
    RUN_TEST(test_decode_tx_length_value);
}

/*******************************************************************************
 * eaarlio_tld_decode_rx_length
 *
 * eaarlio_error eaarlio_tld_decode_rx_length(unsigned char const *buffer,
 *     uint32_t buffer_len, uint16_t *length);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_decode_rx_length_sanity()
{
    unsigned char const buf[2] = { 0 };
    uint16_t length;
    eaarlio_tld_decode_rx_length((unsigned char const *)buf, 2, &length);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_decode_rx_length_null_buf()
{
    uint16_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_tld_decode_rx_length(NULL, 2, &length));
    PASS();
}

/* Can we handle a null length? */
TEST test_decode_rx_length_null_length()
{
    unsigned char const buf[2] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_rx_length((unsigned char const *)buf, 2, NULL));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_decode_rx_length_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    uint16_t length;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_rx_length((unsigned char const *)buf, 0, &length));
    PASS();
}

/* Can we handle a buffer size of 1?
 * 2 is the minimal length needed to parse a length */
TEST test_decode_rx_length_buf_len1()
{
    unsigned char const buf[1] = { 0 };
    uint16_t length;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_rx_length((unsigned char const *)buf, 1, &length));
    PASS();
}

/* Can we parse the length correctly? */
TEST test_decode_rx_length_value()
{
    unsigned char const buf[] = { '\x01', '\x02' };
    uint16_t length;
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_tld_decode_rx_length((unsigned char const *)buf, 2, &length));
    ASSERT_EQ_FMT(513, length, "%d");
    PASS();
}

SUITE(suite_decode_rx_length)
{
    RUN_TEST(test_decode_rx_length_sanity);
    RUN_TEST(test_decode_rx_length_null_buf);
    RUN_TEST(test_decode_rx_length_null_length);
    RUN_TEST(test_decode_rx_length_buf_len0);
    RUN_TEST(test_decode_rx_length_buf_len1);
    RUN_TEST(test_decode_rx_length_value);
}

/*******************************************************************************
 * eaarlio_tld_decode_waveform
 *
 * eaarlio_error eaarlio_tld_decode_waveform(unsigned char const *buffer,
 *     uint32_t buffer_len, unsigned char *waveform, uint16_t waveform_len);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_decode_waveform_sanity()
{
    unsigned char const buf[5] = { 0 };
    unsigned char wf[5];
    eaarlio_tld_decode_waveform(
        (unsigned char const *)buf, 5, (unsigned char *)wf, 5);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_decode_waveform_null_buf()
{
    unsigned char wf[5];
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_waveform(NULL, 5, (unsigned char *)wf, 5));
    PASS();
}

/* Can we handle a null waveform? */
TEST test_decode_waveform_null_wf()
{
    unsigned char const buf[5] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_tld_decode_waveform((unsigned char const *)buf, 5, NULL, 5));
    PASS();
}

/* Can we handle a buffer size of 0? */
TEST test_decode_waveform_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    unsigned char wf[5];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_waveform(
            (unsigned char const *)buf, 0, (unsigned char *)wf, 5));
    PASS();
}

/* Can we handle a waveform size of 0? */
TEST test_decode_waveform_wf_len0()
{
    unsigned char const buf[5] = { 0 };
    unsigned char wf[1];
    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_decode_waveform(
        (unsigned char const *)buf, 5, (unsigned char *)wf, 0));
    PASS();
}

/* Can we handle a shorter buffer than waveform? */
TEST test_decode_waveform_buf_short()
{
    unsigned char const buf[5] = { 0 };
    unsigned char wf[8];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_tld_decode_waveform(
            (unsigned char const *)buf, 5, (unsigned char *)wf, 8));
    PASS();
}

/* Can we read the waveform correctly?? */
TEST test_decode_waveform_value()
{
    unsigned char const buf[] = { '\x01', '\x02', '\x03', '\x04', '\x05' };
    unsigned char wf[5];
    ASSERT_EAARLIO_SUCCESS(eaarlio_tld_decode_waveform(
        (unsigned char const *)buf, 5, (unsigned char *)wf, 5));
    ASSERT_EQ_FMT(1, wf[0], "%d");
    ASSERT_EQ_FMT(2, wf[1], "%d");
    ASSERT_EQ_FMT(3, wf[2], "%d");
    ASSERT_EQ_FMT(4, wf[3], "%d");
    ASSERT_EQ_FMT(5, wf[4], "%d");
    PASS();
}

SUITE(suite_decode_waveform)
{
    RUN_TEST(test_decode_waveform_sanity);
    RUN_TEST(test_decode_waveform_null_buf);
    RUN_TEST(test_decode_waveform_null_wf);
    RUN_TEST(test_decode_waveform_buf_len0);
    RUN_TEST(test_decode_waveform_wf_len0);
    RUN_TEST(test_decode_waveform_buf_short);
    RUN_TEST(test_decode_waveform_value);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_decode_record_header);
    RUN_SUITE(suite_decode_raster_header);
    RUN_SUITE(suite_decode_pulse_header);
    RUN_SUITE(suite_decode_wf_data_length);
    RUN_SUITE(suite_decode_tx_length);
    RUN_SUITE(suite_decode_rx_length);
    RUN_SUITE(suite_decode_waveform);

    GREATEST_MAIN_END();
}
