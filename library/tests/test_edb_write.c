#include "eaarlio/edb.h"
#include "eaarlio/edb_decode.h"
#include "eaarlio/edb_encode.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/edb_write.h"
#include "eaarlio/error.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_stream.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "data_edb.c"

#ifdef _MSC_VER
/* Visual Studio complains about initializing structs with references to
 * non-constant values because it's non-standard, but it's supported on all
 * current target platforms */
#pragma warning(disable : 4204)
#endif

/*******************************************************************************
 * Setup and teardown for test cases
 ******************************************************************************/

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
 * eaarlio_edb_write
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_write_sanity()
{
    eaarlio_edb_write(NULL, NULL);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_write_null_stream()
{
    struct eaarlio_edb edb = eaarlio_edb_empty();

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_write(NULL, &edb));
    PASS();
}

/* Do we properly reject a null pointer for edb? */
TEST test_write_null_edb()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_write(&stream, NULL));
    PASS();
}

/* Do we properly write the file? */
TEST test_write_vals(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb edb = eaarlio_edb_empty();
    struct eaarlio_edb_record records[] = { dec_record_sample1,
        dec_record_sample2, dec_record_sample3, dec_record_sample4,
        dec_record_sample5 };
    const char *files[] = { dec_fn_tld_a, dec_fn_tld_b, dec_fn_tld_c };
    int64_t end_offset = 0;

    edb.record_count = 5;
    edb.file_count = 3;
    edb.records = records;
    edb.files = (char **)files;

    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_write(data->stream, &edb));
    end_offset = data->mock->offset;

    data->mock->offset = 0;
    CHECK_CALL(mock_stream_verify_msg(
        "header", enc_header_sample, data->mock, sizeof enc_header_unique));
    CHECK_CALL(mock_stream_verify_msg("record_sample1", enc_record_sample1,
        data->mock, sizeof enc_record_sample1));
    CHECK_CALL(mock_stream_verify_msg("record_sample2", enc_record_sample2,
        data->mock, sizeof enc_record_sample2));
    CHECK_CALL(mock_stream_verify_msg("record_sample3", enc_record_sample3,
        data->mock, sizeof enc_record_sample3));
    CHECK_CALL(mock_stream_verify_msg("record_sample4", enc_record_sample4,
        data->mock, sizeof enc_record_sample4));
    CHECK_CALL(mock_stream_verify_msg("record_sample5", enc_record_sample5,
        data->mock, sizeof enc_record_sample5));
    CHECK_CALL(mock_stream_verify_msg(
        "fn_tld_a", enc_fn_tld_a, data->mock, sizeof enc_fn_tld_a));
    CHECK_CALL(mock_stream_verify_msg(
        "fn_tld_b", enc_fn_tld_b, data->mock, sizeof enc_fn_tld_b));
    CHECK_CALL(mock_stream_verify_msg(
        "fn_tld_c", enc_fn_tld_c, data->mock, sizeof enc_fn_tld_c));
    ASSERT_EQ_FMT(data->mock->offset, end_offset, "%d");

    PASS();
}

SUITE(suite_write)
{
    struct test_data data;
    data.size = 200;

    RUN_TEST(test_write_sanity);

    RUN_TEST(test_write_null_stream);
    RUN_TEST(test_write_null_edb);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    RUN_TEST1(test_write_vals, &data);
}

/*******************************************************************************
 * eaarlio_edb_write_header
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_write_header_sanity()
{
    eaarlio_edb_write_header(NULL, NULL);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_write_header_null_stream()
{
    struct eaarlio_edb_header header = { 0, 0, 0 };

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_write_header(NULL, &header));
    PASS();
}

/* Do we properly reject a null pointer for header? */
TEST test_write_header_null_header()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_write_header(&stream, NULL));
    PASS();
}

/* Do we properly respond to write errors? */
TEST test_write_header_write_fail(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb_header got = { 0, 0, 0 };

    data->mock->no_write = 1;

    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_WRITE_ERROR,
        eaarlio_edb_write_header(data->stream, &got));

    PASS();
}

/* Do we properly write the header? */
TEST test_write_header_vals(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    int64_t end_offset = 0;

    ASSERT_EAARLIO_SUCCESS(
        eaarlio_edb_write_header(data->stream, &dec_header_unique));
    end_offset = data->mock->offset;

    data->mock->offset = 0;
    CHECK_CALL(mock_stream_verify(
        enc_header_unique, data->mock, sizeof enc_header_unique));
    ASSERT_EQ_FMT(data->mock->offset, end_offset, "%d");

    PASS();
}

SUITE(suite_write_header)
{
    struct test_data data;
    data.size = EAARLIO_EDB_HEADER_SIZE;

    RUN_TEST(test_write_header_sanity);

    RUN_TEST(test_write_header_null_stream);
    RUN_TEST(test_write_header_null_header);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    RUN_TEST1(test_write_header_write_fail, &data);
    RUN_TEST1(test_write_header_vals, &data);
}

/*******************************************************************************
 * eaarlio_edb_write_records
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_write_records_sanity()
{
    eaarlio_edb_write_records(NULL, NULL, NULL);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_write_records_null_stream()
{
    struct eaarlio_edb_header header;
    struct eaarlio_edb_record records;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_write_records(NULL, &header, &records));
    PASS();
}

/* Do we properly reject a null pointer for header? */
TEST test_write_records_null_header()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb_record records;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_write_records(&stream, NULL, &records));
    PASS();
}

/* Do we properly reject a null pointer for records? */
TEST test_write_records_null_records()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb_header header;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_write_records(&stream, &header, NULL));
    PASS();
}

/* Do we properly respond to write errors? */
TEST test_write_records_write_fail(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb_record got[2] = {
        { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0 },
    };
    struct eaarlio_edb_header header = { 0, 2, 0 };

    data->mock->no_write = 1;

    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_WRITE_ERROR,
        eaarlio_edb_write_records(
            data->stream, &header, (struct eaarlio_edb_record *)&got));

    PASS();
}

/* Do we properly write the records? */
TEST test_write_records_vals(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb_header header = dec_header_sample;
    struct eaarlio_edb_record records[] = { dec_record_sample1,
        dec_record_sample2, dec_record_sample3, dec_record_sample4,
        dec_record_sample5 };
    int64_t end_offset = 0;

    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_write_records(
        data->stream, &header, (struct eaarlio_edb_record *)&records));
    end_offset = data->mock->offset;

    data->mock->offset = 0;
    mock_stream_verify_fixed(0, data->mock, EAARLIO_EDB_HEADER_SIZE);
    CHECK_CALL(mock_stream_verify_msg("record_sample1", enc_record_sample1,
        data->mock, sizeof enc_record_sample1));
    CHECK_CALL(mock_stream_verify_msg("record_sample2", enc_record_sample2,
        data->mock, sizeof enc_record_sample2));
    CHECK_CALL(mock_stream_verify_msg("record_sample3", enc_record_sample3,
        data->mock, sizeof enc_record_sample3));
    CHECK_CALL(mock_stream_verify_msg("record_sample4", enc_record_sample4,
        data->mock, sizeof enc_record_sample4));
    CHECK_CALL(mock_stream_verify_msg("record_sample5", enc_record_sample5,
        data->mock, sizeof enc_record_sample5));
    ASSERT_EQ_FMT(data->mock->offset, end_offset, "%d");

    PASS();
}

SUITE(suite_write_records)
{
    struct test_data data;
    data.size = EAARLIO_EDB_HEADER_SIZE + EAARLIO_EDB_RECORD_SIZE * 5;

    RUN_TEST(test_write_records_sanity);

    RUN_TEST(test_write_records_null_stream);
    RUN_TEST(test_write_records_null_header);
    RUN_TEST(test_write_records_null_records);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    RUN_TEST1(test_write_records_write_fail, &data);
    RUN_TEST1(test_write_records_vals, &data);
}

/*******************************************************************************
 * eaarlio_edb_write_filenames
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_write_filenames_sanity()
{
    eaarlio_edb_write_filenames(NULL, NULL, NULL);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_write_filenames_null_stream()
{
    struct eaarlio_edb_header header = { 0, 0, 0 };
    char *files = NULL;

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_write_filenames(NULL, &header,
                                         (const char *const *)&files));
    PASS();
}

/* Do we properly reject a null pointer for header? */
TEST test_write_filenames_null_header()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    char *files = NULL;

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_write_filenames(&stream, NULL,
                                         (const char *const *)&files));
    PASS();
}

/* Do we properly reject a null pointer for files? */
TEST test_write_filenames_null_files()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb_header header = { 0, 0, 0 };

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_write_filenames(&stream, &header, NULL));
    PASS();
}

/* Do we properly write files? */
TEST test_write_filenames_vals(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    const char *files[] = { dec_fn_tld_a, dec_fn_tld_b, dec_fn_tld_c };
    struct eaarlio_edb_header header = dec_header_sample;
    int64_t end_offset = 0;

    ASSERT_EAARLIO_SUCCESS(
        eaarlio_edb_write_filenames(data->stream, &header, files));
    end_offset = data->mock->offset;

    data->mock->offset = 0;
    mock_stream_verify_fixed(0, data->mock, header.files_offset);
    CHECK_CALL(mock_stream_verify_msg(
        "fn_tld_a", enc_fn_tld_a, data->mock, sizeof enc_fn_tld_a));
    CHECK_CALL(mock_stream_verify_msg(
        "fn_tld_b", enc_fn_tld_b, data->mock, sizeof enc_fn_tld_b));
    CHECK_CALL(mock_stream_verify_msg(
        "fn_tld_c", enc_fn_tld_c, data->mock, sizeof enc_fn_tld_c));
    ASSERT_EQ_FMT(data->mock->offset, end_offset, "%d");

    PASS();
}

SUITE(suite_write_filenames)
{
    struct test_data data;
    data.size = 200;

    RUN_TEST(test_write_filenames_sanity);

    RUN_TEST(test_write_filenames_null_stream);
    RUN_TEST(test_write_filenames_null_header);
    RUN_TEST(test_write_filenames_null_files);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    char *files[3] = { NULL, NULL, NULL };
    data.extra = &files;
    RUN_TEST1(test_write_filenames_vals, &data);
    if(files[0])
        free(files[0]);
    if(files[1])
        free(files[1]);
    if(files[2])
        free(files[2]);
    data.extra = NULL;
}

/*******************************************************************************
 * Run test suites
 ******************************************************************************/

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_write);
    RUN_SUITE(suite_write_header);
    RUN_SUITE(suite_write_records);
    RUN_SUITE(suite_write_filenames);

    GREATEST_MAIN_END();
}
