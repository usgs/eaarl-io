#include "eaarlio/edb.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/edb_read.h"
#include "eaarlio/error.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_stream.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "data_edb.c"

#ifdef _MSC_VER
/* Visual Studio complains about initializing structs with references to local
 * variables because it's non-standard, but it's supported on all current
 * target platforms */
#pragma warning(disable : 4221)
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
    data->mock->no_write = 1;
}

static void teardown_data_cb(void *arg)
{
    struct test_data *data = (struct test_data *)arg;

    mock_stream_stream_destroy(data->stream);
    data->stream = NULL;

    mock_stream_destroy(data->mock);
    data->mock = NULL;
}

static void setup_edb_cb(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    setup_data_cb(arg);

    struct eaarlio_edb *edb = (struct eaarlio_edb *)data->extra;
    edb->record_count = 0;
    edb->file_count = 0;
    edb->records = NULL;
    edb->files = NULL;
}

static void teardown_edb_cb(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    teardown_data_cb(arg);

    struct eaarlio_edb *edb = (struct eaarlio_edb *)data->extra;

    if(edb->records)
        free(edb->records);
    if(edb->files) {
        if(edb->files[0])
            free(edb->files[0]);
        if(edb->files[1])
            free(edb->files[1]);
        if(edb->files[2])
            free(edb->files[2]);
        free(edb->files);
    }
}

/*******************************************************************************
 * eaarlio_edb_read
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_read_sanity()
{
    eaarlio_edb_read(NULL, NULL, NULL, 0, 0);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_read_null_stream()
{
    struct eaarlio_edb edb = eaarlio_edb_empty();

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_read(NULL, &edb, NULL, 0, 0));
    PASS();
}

/* Do we properly reject a null pointer for edb? */
TEST test_read_null_edb()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_read(&stream, NULL, NULL, 0, 0));
    PASS();
}

/* Do we properly read the file? */
TEST test_read_vals(void *arg, int include_records, int include_files)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb *got = (struct eaarlio_edb *)data->extra;

    data->mock->offset = 0;
    mock_stream_write(data->mock, sizeof enc_header_sample, enc_header_sample);
    mock_stream_write(
        data->mock, sizeof enc_record_sample1, enc_record_sample1);
    mock_stream_write(
        data->mock, sizeof enc_record_sample2, enc_record_sample2);
    mock_stream_write(
        data->mock, sizeof enc_record_sample3, enc_record_sample3);
    mock_stream_write(
        data->mock, sizeof enc_record_sample4, enc_record_sample4);
    mock_stream_write(
        data->mock, sizeof enc_record_sample5, enc_record_sample5);
    mock_stream_write(data->mock, sizeof enc_fn_tld_a, enc_fn_tld_a);
    mock_stream_write(data->mock, sizeof enc_fn_tld_b, enc_fn_tld_b);
    mock_stream_write(data->mock, sizeof enc_fn_tld_c, enc_fn_tld_c);
    data->mock->offset = 0;

    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_read(
        data->stream, got, NULL, include_records, include_files));

    ASSERT_EQ_FMT(dec_header_sample.record_count, got->record_count, "%d");
    ASSERT_EQ_FMT(dec_header_sample.file_count, got->file_count, "%d");

    if(include_records) {
        ASSERT(got->records);
        CHECK_CALL(check_record(&dec_record_sample1, &got->records[0]));
        CHECK_CALL(check_record(&dec_record_sample2, &got->records[1]));
        CHECK_CALL(check_record(&dec_record_sample3, &got->records[2]));
        CHECK_CALL(check_record(&dec_record_sample4, &got->records[3]));
        CHECK_CALL(check_record(&dec_record_sample5, &got->records[4]));
    } else {
        ASSERT_FALSE(got->records);
    }

    if(include_files) {
        ASSERT(got->files);
        ASSERT(got->files[0]);
        ASSERT_STR_EQ(dec_fn_tld_a, got->files[0]);
        ASSERT(got->files[1]);
        ASSERT_STR_EQ(dec_fn_tld_b, got->files[1]);
        ASSERT(got->files[2]);
        ASSERT_STR_EQ(dec_fn_tld_c, got->files[2]);
    } else {
        ASSERT_FALSE(got->files);
    }

    PASS();
}

TEST test_read_vals_all(void *arg)
{
    CHECK_CALL(test_read_vals(arg, 1, 1));
    PASS();
}

TEST test_read_vals_none(void *arg)
{
    CHECK_CALL(test_read_vals(arg, 0, 0));
    PASS();
}

TEST test_read_vals_records(void *arg)
{
    CHECK_CALL(test_read_vals(arg, 1, 0));
    PASS();
}

TEST test_read_vals_files(void *arg)
{
    CHECK_CALL(test_read_vals(arg, 0, 1));
    PASS();
}

SUITE(suite_read)
{
    struct test_data data;
    data.size = 200;

    RUN_TEST(test_read_sanity);

    RUN_TEST(test_read_null_stream);
    RUN_TEST(test_read_null_edb);

    SET_SETUP(setup_edb_cb, &data);
    SET_TEARDOWN(teardown_edb_cb, &data);

    struct eaarlio_edb edb = eaarlio_edb_empty();
    data.extra = &edb;

    RUN_TEST1(test_read_vals_all, &data);
    RUN_TEST1(test_read_vals_none, &data);
    RUN_TEST1(test_read_vals_records, &data);
    RUN_TEST1(test_read_vals_files, &data);

    data.extra = NULL;
}

/*******************************************************************************
 * eaarlio_edb_read_header
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_read_header_sanity()
{
    eaarlio_edb_read_header(NULL, NULL);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_read_header_null_stream()
{
    struct eaarlio_edb_header header = { 0, 0, 0 };

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_read_header(NULL, &header));
    PASS();
}

/* Do we properly reject a null pointer for header? */
TEST test_read_header_null_header()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();

    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_edb_read_header(&stream, NULL));
    PASS();
}

/* Do we properly respond to read errors? */
TEST test_read_header_read_fail(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb_header got = { 0, 0, 0 };

    data->mock->no_read = 1;

    ASSERT_EAARLIO_ERR(
        EAARLIO_STREAM_READ_ERROR, eaarlio_edb_read_header(data->stream, &got));

    PASS();
}

/* Do we properly read the header? */
TEST test_read_header_vals(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb_header got = { 0, 0, 0 };

    data->mock->offset = 0;
    mock_stream_write(data->mock, sizeof enc_header_unique, enc_header_unique);
    data->mock->offset = 0;

    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_read_header(data->stream, &got));

    CHECK_CALL(check_header(&dec_header_unique, &got));

    PASS();
}

SUITE(suite_read_header)
{
    struct test_data data;
    data.size = EAARLIO_EDB_HEADER_SIZE;

    RUN_TEST(test_read_header_sanity);

    RUN_TEST(test_read_header_null_stream);
    RUN_TEST(test_read_header_null_header);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    RUN_TEST1(test_read_header_read_fail, &data);
    RUN_TEST1(test_read_header_vals, &data);
}

/*******************************************************************************
 * eaarlio_edb_read_records
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_read_records_sanity()
{
    eaarlio_edb_read_records(NULL, NULL, NULL);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_read_records_null_stream()
{
    struct eaarlio_edb_header header;
    struct eaarlio_edb_record records;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_read_records(NULL, &header, &records));
    PASS();
}

/* Do we properly reject a null pointer for header? */
TEST test_read_records_null_header()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb_record records;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_read_records(&stream, NULL, &records));
    PASS();
}

/* Do we properly reject a null pointer for records? */
TEST test_read_records_null_records()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb_header header;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_read_records(&stream, &header, NULL));
    PASS();
}

/* Do we properly respond to read errors? */
TEST test_read_records_read_fail(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb_record got[2];
    struct eaarlio_edb_header header = { 0, 2, 0 };

    data->mock->no_read = 1;

    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_READ_ERROR,
        eaarlio_edb_read_records(
            data->stream, &header, (struct eaarlio_edb_record *)&got));

    PASS();
}

/* Do we properly read the records? */
TEST test_read_records_vals(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    struct eaarlio_edb_record got[5];
    struct eaarlio_edb_header header = dec_header_sample;

    data->mock->offset = EAARLIO_EDB_HEADER_SIZE;
    mock_stream_write(
        data->mock, sizeof enc_record_sample1, enc_record_sample1);
    mock_stream_write(
        data->mock, sizeof enc_record_sample2, enc_record_sample2);
    mock_stream_write(
        data->mock, sizeof enc_record_sample3, enc_record_sample3);
    mock_stream_write(
        data->mock, sizeof enc_record_sample4, enc_record_sample4);
    mock_stream_write(
        data->mock, sizeof enc_record_sample5, enc_record_sample5);
    data->mock->offset = 0;

    ASSERT_EAARLIO_SUCCESS(eaarlio_edb_read_records(
        data->stream, &header, (struct eaarlio_edb_record *)&got));

    CHECK_CALL(check_record(&dec_record_sample1, &got[0]));
    CHECK_CALL(check_record(&dec_record_sample2, &got[1]));
    CHECK_CALL(check_record(&dec_record_sample3, &got[2]));
    CHECK_CALL(check_record(&dec_record_sample4, &got[3]));
    CHECK_CALL(check_record(&dec_record_sample5, &got[4]));

    PASS();
}

SUITE(suite_read_records)
{
    struct test_data data;
    data.size = EAARLIO_EDB_HEADER_SIZE + EAARLIO_EDB_RECORD_SIZE * 5;

    RUN_TEST(test_read_records_sanity);

    RUN_TEST(test_read_records_null_stream);
    RUN_TEST(test_read_records_null_header);
    RUN_TEST(test_read_records_null_records);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    RUN_TEST1(test_read_records_read_fail, &data);
    RUN_TEST1(test_read_records_vals, &data);
}

/*******************************************************************************
 * eaarlio_edb_read_filenames
 ******************************************************************************/

/* Make sure we don't seg fault. */
TEST test_read_filenames_sanity()
{
    eaarlio_edb_read_filenames(NULL, NULL, NULL, NULL);
    PASS();
}

/* Do we properly reject a null pointer for stream? */
TEST test_read_filenames_null_stream()
{
    struct eaarlio_edb_header header = { 0, 0, 0 };
    char *files = NULL;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_read_filenames(NULL, &header, &files, NULL));
    PASS();
}

/* Do we properly reject a null pointer for header? */
TEST test_read_filenames_null_header()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    char *files = NULL;

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_read_filenames(&stream, NULL, &files, NULL));
    PASS();
}

/* Do we properly reject a null pointer for files? */
TEST test_read_filenames_null_files()
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    struct eaarlio_edb_header header = { 0, 0, 0 };

    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_edb_read_filenames(&stream, &header, NULL, NULL));
    PASS();
}

/* Do we properly read files? */
TEST test_read_filenames_vals(void *arg)
{
    struct test_data *data = (struct test_data *)arg;
    char **files = (char **)data->extra;

    data->mock->offset = dec_header_sample.files_offset;
    mock_stream_write(data->mock, sizeof enc_fn_tld_a, enc_fn_tld_a);
    mock_stream_write(data->mock, sizeof enc_fn_tld_b, enc_fn_tld_b);
    mock_stream_write(data->mock, sizeof enc_fn_tld_c, enc_fn_tld_c);
    data->mock->offset = 0;

    struct eaarlio_edb_header header = dec_header_sample;

    ASSERT_EAARLIO_SUCCESS(
        eaarlio_edb_read_filenames(data->stream, &header, files, NULL));

    ASSERT(files[0]);
    ASSERT_STR_EQ(dec_fn_tld_a, files[0]);
    ASSERT(files[1]);
    ASSERT_STR_EQ(dec_fn_tld_b, files[1]);
    ASSERT(files[2]);
    ASSERT_STR_EQ(dec_fn_tld_c, files[2]);

    PASS();
}

SUITE(suite_read_filenames)
{
    struct test_data data;
    data.size = 200;

    RUN_TEST(test_read_filenames_sanity);

    RUN_TEST(test_read_filenames_null_stream);
    RUN_TEST(test_read_filenames_null_header);
    RUN_TEST(test_read_filenames_null_files);

    SET_SETUP(setup_data_cb, &data);
    SET_TEARDOWN(teardown_data_cb, &data);

    char *files[3] = { NULL, NULL, NULL };
    data.extra = &files;
    RUN_TEST1(test_read_filenames_vals, &data);
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

    RUN_SUITE(suite_read);
    RUN_SUITE(suite_read_header);
    RUN_SUITE(suite_read_records);
    RUN_SUITE(suite_read_filenames);

    GREATEST_MAIN_END();
}
