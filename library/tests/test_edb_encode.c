#include "eaarlio/edb.h"
#include "eaarlio/edb_encode.h"
#include "eaarlio/error.h"
#include "greatest.h"
#include "assert_error.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "data_edb.c"

/*******************************************************************************
 * eaarlio_edb_encode_header
 *
 * eaarlio_error eaarlio_edb_encode_header(unsigned char *buffer,
 *     uint32_t buffer_len, struct eaarlio_edb_header const *header);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_header_sanity()
{
    unsigned char buf[12];
    struct eaarlio_edb_header const header = { 0, 0, 0 };
    eaarlio_edb_encode_header((unsigned char *)buf, 12, &header);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_header_null_buf()
{
    struct eaarlio_edb_header const header = { 0, 0, 0 };
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_encode_header(NULL, 12, &header));
    PASS();
}

/* Can we handle a null header? */
TEST test_header_null_header()
{
    unsigned char buf[12];
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_edb_encode_header((unsigned char *)buf, 12, NULL));
    PASS();
}

/* Can we handle a handle a buffer size of 0? */
TEST test_header_buf_len0()
{
    unsigned char buf[1];
    struct eaarlio_edb_header const header = { 0, 0, 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_encode_header((unsigned char *)buf, 0, &header));
    PASS();
}

/* Can we handle a buffer size of 11
 * 12 is the minimal size to write a header */
TEST test_header_buf_len11()
{
    unsigned char buf[11];
    struct eaarlio_edb_header const header = { 0, 0, 0 };
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_encode_header((unsigned char *)buf, 11, &header));
    PASS();
}

/* Can we encode all fields correctly? */
TEST test_header_values()
{
    unsigned char got[sizeof enc_header_unique];
    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_encode_header(
        (unsigned char *)got, sizeof got, &dec_header_unique));
    ASSERT_MEM_EQ(enc_header_unique, got, sizeof got);
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
 * eaarlio_edb_encode_record
 *
 * eaarlio_error eaarlio_edb_encode_record(unsigned char *buffer,
 *     uint32_t buffer_len, struct eaarlio_edb_record const *record);
 *******************************************************************************
 */

/* Make sure that we don't seg fault or get truly bizarre breaks */
TEST test_record_sanity()
{
    unsigned char buf[20];
    struct eaarlio_edb_record const record = eaarlio_edb_record_empty();
    eaarlio_edb_encode_record((unsigned char *)buf, 20, &record);
    PASS();
}

/* Can we handle a null buffer? */
TEST test_record_null_buf()
{
    struct eaarlio_edb_record const record = eaarlio_edb_record_empty();
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_encode_record(NULL, 12, &record));
    PASS();
}

/* Can we handle a null record? */
TEST test_record_null_record()
{
    unsigned char buf[12];
    ASSERT_EAARLIO_ERR(EAARLIO_NULL,
        eaarlio_edb_encode_record((unsigned char *)buf, 12, NULL));
    PASS();
}

/* Can we handle a handle a buffer size of 0? */
TEST test_record_buf_len0()
{
    unsigned char buf[1];
    struct eaarlio_edb_record const record = eaarlio_edb_record_empty();
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_encode_record((unsigned char *)buf, 0, &record));
    PASS();
}

/* Can we handle a buffer size of 19
 * 20 is the minimal size to write a record */
TEST test_record_buf_len19()
{
    unsigned char buf[19];
    struct eaarlio_edb_record const record = eaarlio_edb_record_empty();
    ASSERT_EAARLIO_ERR(EAARLIO_BUFFER_SHORT,
        eaarlio_edb_encode_record((unsigned char *)buf, 19, &record));
    PASS();
}

/* Can we encode all fields correctly? */
TEST test_record_values()
{
    unsigned char got[sizeof enc_record_unique];
    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_encode_record(
        (unsigned char *)got, sizeof got, &dec_record_unique));
    ASSERT_MEM_EQ(enc_record_unique, got, sizeof got);
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
 * eaarlio_edb_encode_filename_length
 *
 * eaarlio_error eaarlio_edb_encode_filename_length(unsigned char *buffer,
 *     uint32_t buffer_len, uint16_t length);
 *******************************************************************************
 */

TEST test_filename_length_sanity()
{
    eaarlio_edb_encode_filename_length(NULL, 0, 0);
    PASS();
}

TEST test_filename_length_value()
{
    unsigned char buf[2] = { 0 };
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_edb_encode_filename_length((unsigned char *)buf, 2, 513U));
    ASSERT_EQ_FMT(0x01U, buf[0], "%02x");
    ASSERT_EQ_FMT(0x02U, buf[1], "%02x");
    PASS();
}

SUITE(suite_filename_length)
{
    RUN_TEST(test_filename_length_sanity);
    RUN_TEST(test_filename_length_value);
}

/*******************************************************************************
 * eaarlio_edb_encode_filename
 *
 * eaarlio_error eaarlio_edb_encode_filename(unsigned char *buffer,
 *     uint32_t buffer_len, char const *filename);
 *******************************************************************************
 */

TEST test_filename_sanity()
{
    eaarlio_edb_encode_filename(NULL, 0, NULL);
    PASS();
}

TEST test_filename_value()
{
    unsigned char buf[5] = { 0 };
    ASSERT_EAARLIO_SUCCESS(
        eaarlio_edb_encode_filename((unsigned char *)buf, 5, "abcde"));
    ASSERT_EQ_FMT(0x61U, buf[0], "%02x");
    ASSERT_EQ_FMT(0x62U, buf[1], "%02x");
    ASSERT_EQ_FMT(0x63U, buf[2], "%02x");
    ASSERT_EQ_FMT(0x64U, buf[3], "%02x");
    ASSERT_EQ_FMT(0x65U, buf[4], "%02x");
    PASS();
}

SUITE(suite_filename)
{
    RUN_TEST(test_filename_sanity);
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
