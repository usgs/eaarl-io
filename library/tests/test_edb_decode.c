#include "eaarlio/edb.h"
#include "eaarlio/edb_decode.h"
#include "eaarlio/error.h"
#include "greatest.h"
#include "assert_error.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "data_edb.c"

/*******************************************************************************
 * eaarlio_edb_decode_header
 *
 * eaarlio_error eaarlio_edb_decode_header(unsigned char const *buffer,
 *     uint32_t buffer_len, struct eaarlio_edb_header *header);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_header_sanity()
{
    unsigned char const buf[12] = { 0 };
    struct eaarlio_edb_header header;
    eaarlio_edb_decode_header((unsigned char const *)buf, 12, &header);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_header_null_buf()
{
    struct eaarlio_edb_header header;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_decode_header(NULL, 12, &header));
    PASS();
}

/* Can we handle a null header? */
TEST test_header_null_header()
{
    unsigned char const buf[12] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_edb_decode_header((unsigned char const *)buf, 12, NULL));
    PASS();
}

/* Can we handle a handle a buffer size of 0? */
TEST test_header_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    struct eaarlio_edb_header header;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_decode_header((unsigned char const *)buf, 0, &header));
    PASS();
}

/* Can we handle a buffer size of 11
 * 12 is the minimal size to read a header */
TEST test_header_buf_len11()
{
    unsigned char const buf[11] = { 0 };
    struct eaarlio_edb_header header;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_decode_header((unsigned char const *)buf, 11, &header));
    PASS();
}

/* Can we parse all fields correctly? */
TEST test_header_values()
{
    struct eaarlio_edb_header got;
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_edb_decode_header((unsigned char const *)enc_header_unique,
            sizeof enc_header_unique, &got));
    CHECK_CALL(check_header(&dec_header_unique, &got));
    PASS();
}

SUITE(suite_header)
{
    RUN_TEST(test_header_sanity);
    RUN_TEST(test_header_null_buf);
    RUN_TEST(test_header_null_header);
    RUN_TEST(test_header_buf_len0);
    RUN_TEST(test_header_buf_len11);
    RUN_TEST(test_header_values);
}

/*******************************************************************************
 * eaarlio_edb_decode_record
 *
 * eaarlio_error eaarlio_edb_decode_record(unsigned char const *buffer,
 *     uint32_t buffer_len, struct eaarlio_edb_record *record);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_record_sanity()
{
    unsigned char const buf[20] = { 0 };
    struct eaarlio_edb_record record;
    eaarlio_edb_decode_record((unsigned char const *)buf, 20, &record);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_record_null_buf()
{
    struct eaarlio_edb_record record;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_decode_record(NULL, 12, &record));
    PASS();
}

/* Can we handle a null record? */
TEST test_record_null_record()
{
    unsigned char const buf[20] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_edb_decode_record((unsigned char const *)buf, 20, NULL));
    PASS();
}

/* Can we handle a handle a buffer size of 0? */
TEST test_record_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    struct eaarlio_edb_record record;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_decode_record((unsigned char const *)buf, 0, &record));
    PASS();
}

/* Can we handle a buffer size of 19
 * 20 is the minimal size to read a record */
TEST test_record_buf_len19()
{
    unsigned char const buf[19] = { 0 };
    struct eaarlio_edb_record record;
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_decode_record((unsigned char const *)buf, 19, &record));
    PASS();
}

/* Can we parse all fields correctly? */
TEST test_record_values()
{
    struct eaarlio_edb_record got;
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_edb_decode_record((unsigned char const *)enc_record_unique,
            sizeof enc_record_unique, &got));
    CHECK_CALL(check_record(&dec_record_unique, &got));
    PASS();
}

SUITE(suite_record)
{
    RUN_TEST(test_record_sanity);
    RUN_TEST(test_record_null_buf);
    RUN_TEST(test_record_null_record);
    RUN_TEST(test_record_buf_len0);
    RUN_TEST(test_record_buf_len19);
    RUN_TEST(test_record_values);
}

/*******************************************************************************
 * eaarlio_edb_decode_filename_length
 *
 * eaarlio_error eaarlio_edb_decode_filename_length(unsigned char const *buffer,
 *     uint32_t buffer_len, uint16_t *length);
 *******************************************************************************
 */

TEST test_filename_length_sanity()
{
    eaarlio_edb_decode_filename_length(NULL, 0, NULL);
    PASS();
}

TEST test_filename_length_null_buf()
{
    uint16_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_decode_filename_length(NULL, 2, &length));
    PASS();
}

TEST test_filename_length_null_length()
{
    unsigned char const buf[2] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_decode_filename_length(
                                         (unsigned char const *)buf, 2, NULL));
    PASS();
}

TEST test_filename_length_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    uint16_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_edb_decode_filename_length(
                                  (unsigned char const *)buf, 0, &length));
    PASS();
}

TEST test_filename_length_buf_len1()
{
    unsigned char const buf[1] = { 0 };
    uint16_t length;
    ASSERT_EAARLIO_ERR(
        EAARLIO_BUFFER_SHORT, eaarlio_edb_decode_filename_length(
                                  (unsigned char const *)buf, 1, &length));
    PASS();
}

TEST test_filename_length_value()
{
    unsigned char const buf[] = { '\x01', '\x02' };
    uint16_t length;
    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_decode_filename_length(
        (unsigned char const *)buf, 2, &length));
    ASSERT_EQ_FMT(513, length, "%d");
    PASS();
}

SUITE(suite_filename_length)
{
    RUN_TEST(test_filename_length_sanity);
    RUN_TEST(test_filename_length_null_buf);
    RUN_TEST(test_filename_length_null_length);
    RUN_TEST(test_filename_length_buf_len0);
    RUN_TEST(test_filename_length_buf_len1);
    RUN_TEST(test_filename_length_value);
}

/*******************************************************************************
 * eaarlio_edb_decode_filename
 *
 * eaarlio_error eaarlio_edb_decode_filename(unsigned char const *buffer,
 *     uint32_t buffer_len, uint16_t length, char *filename,
 *     size_t filename_len);
 *******************************************************************************
 */

TEST test_filename_sanity()
{
    eaarlio_edb_decode_filename(NULL, 0, 0, NULL, 0);
    PASS();
}

TEST test_filename_null_buf()
{
    char fn[5];
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_decode_filename(NULL, 5, 5, (char *)fn, 6));
    PASS();
}

TEST test_filename_null_fn()
{
    unsigned char const buf[5] = { 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_edb_decode_filename((unsigned char const *)buf, 5, 5, NULL, 6));
    PASS();
}

TEST test_filename_buf_len0()
{
    unsigned char const buf[1] = { 0 };
    char fn[5];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_decode_filename(
            (unsigned char const *)buf, 0, 4, (char *)fn, 5));
    PASS();
}

TEST test_filename_fn_len0()
{
    unsigned char const buf[5] = { 0 };
    char fn[1];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_decode_filename(
            (unsigned char const *)buf, 5, 5, (char *)fn, 0));
    PASS();
}

TEST test_filename_len_len0()
{
    unsigned char const buf[5] = { 0 };
    char fn[6];
    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_decode_filename(
        (unsigned char const *)buf, 5, 0, (char *)fn, 6));
    PASS();
}

/* Can we handle a shorter buffer than filename? */
TEST test_filename_buf_short()
{
    unsigned char const buf[5] = { 0 };
    char fn[8];
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_decode_filename(
            (unsigned char const *)buf, 5, 7, (char *)fn, 8));
    PASS();
}

/* Can we read the filename correctly? */
TEST test_filename_value()
{
    unsigned char const buf[] = { '\x01', '\x02', '\x03', '\x04', '\x05' };
    char fn[6] = { 0 };
    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_decode_filename(
        (unsigned char const *)buf, 5, 5, (char *)fn, 6));
    ASSERT_EQ_FMT(1, fn[0], "%d");
    ASSERT_EQ_FMT(2, fn[1], "%d");
    ASSERT_EQ_FMT(3, fn[2], "%d");
    ASSERT_EQ_FMT(4, fn[3], "%d");
    ASSERT_EQ_FMT(5, fn[4], "%d");
    ASSERT_EQ_FMT(0, fn[5], "%d");
    PASS();
}

SUITE(suite_filename)
{
    RUN_TEST(test_filename_sanity);
    RUN_TEST(test_filename_null_buf);
    RUN_TEST(test_filename_null_fn);
    RUN_TEST(test_filename_buf_len0);
    RUN_TEST(test_filename_fn_len0);
    RUN_TEST(test_filename_len_len0);
    RUN_TEST(test_filename_buf_short);
    RUN_TEST(test_filename_value);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_header);
    RUN_SUITE(suite_record);
    RUN_SUITE(suite_filename_length);
    RUN_SUITE(suite_filename);

    GREATEST_MAIN_END();
}
