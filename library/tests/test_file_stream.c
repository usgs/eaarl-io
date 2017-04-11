#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/stream.h"
#include "greatest.h"
#include "assert_error.h"
#include "util_tempfile.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static char const fn[] = DATADIR "/alphanum.txt";
static char const raw[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n";
#define raw_len 63

/*******************************************************************************
 * suite_null
 *******************************************************************************
 */

TEST test_null_sanity()
{
    eaarlio_file_stream(NULL, NULL, NULL);
    PASS();
}

TEST test_null_with_stream()
{
    struct eaarlio_stream stream;
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_file_stream(&stream, NULL, "r"));
    PASS();
}

SUITE(suite_null)
{
    RUN_TEST(test_null_sanity);
    RUN_TEST(test_null_with_stream);
}

/*******************************************************************************
 * suite_read
 *******************************************************************************
 */

static void cb_read_setup(void *arg)
{
    struct eaarlio_stream *stream = (struct eaarlio_stream *)arg;
    *stream = eaarlio_stream_empty();
}

static void cb_read_teardown(void *arg)
{
    struct eaarlio_stream *stream = (struct eaarlio_stream *)arg;
    if(stream->data) {
        FILE *f = (FILE *)stream->data;
        fclose(f);
        stream->data = NULL;
    }
}

TEST test_read_null_stream()
{
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_file_stream(NULL, fn, "r"));
    PASS();
}

TEST test_read_null_mode(struct eaarlio_stream *stream)
{
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, eaarlio_file_stream(stream, fn, NULL));
    PASS();
}

TEST test_read_read_data(struct eaarlio_stream *stream)
{
    unsigned char buf[10];
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_SUCCESS(stream->read(stream, 10, (unsigned char *)&buf));
    ASSERT_MEM_EQ(raw, buf, 10);
    ASSERT_EAARLIO_SUCCESS(stream->close(stream));
    PASS();
}

TEST test_read_seek_tell(struct eaarlio_stream *stream)
{
    unsigned char buf[10];
    int64_t position;

    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_SUCCESS(stream->seek(stream, 5, SEEK_SET));
    ASSERT_EAARLIO_SUCCESS(stream->tell(stream, &position));
    ASSERT_EQ_FMT(5, position, "%d");
    ASSERT_EAARLIO_SUCCESS(stream->read(stream, 10, (unsigned char *)&buf));
    ASSERT_MEM_EQ(raw + 5, buf, 10);
    ASSERT_EAARLIO_SUCCESS(stream->tell(stream, &position));
    ASSERT_EQ_FMT(15, position, "%d");
    ASSERT_EAARLIO_SUCCESS(stream->close(stream));
    PASS();
}

TEST test_read_null_close(struct eaarlio_stream *stream)
{
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, stream->close(NULL));
    PASS();
}

TEST test_read_null_read(struct eaarlio_stream *stream)
{
    unsigned char buf[10];
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, stream->read(NULL, 10, (unsigned char *)&buf));
    PASS();
}

TEST test_read_null_write(struct eaarlio_stream *stream)
{
    unsigned char buf[10];
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, stream->write(NULL, 10, (unsigned char *)&buf));
    PASS();
}

TEST test_read_null_seek(struct eaarlio_stream *stream)
{
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, stream->seek(NULL, 0, SEEK_SET));
    PASS();
}

TEST test_read_null_tell(struct eaarlio_stream *stream)
{
    int64_t position;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, stream->tell(NULL, &position));
    PASS();
}

TEST test_read_invalid_close(struct eaarlio_stream *stream)
{
    struct eaarlio_stream bad = *stream;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    bad.data = NULL;
    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_INVALID, stream->close(&bad));
    PASS();
}

TEST test_read_invalid_read(struct eaarlio_stream *stream)
{
    struct eaarlio_stream bad = *stream;
    unsigned char buf[10];
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    bad.data = NULL;
    ASSERT_EAARLIO_ERR(
        EAARLIO_STREAM_INVALID, stream->read(&bad, 10, (unsigned char *)&buf));
    PASS();
}

TEST test_read_invalid_write(struct eaarlio_stream *stream)
{
    struct eaarlio_stream bad = *stream;
    unsigned char buf[10];
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    bad.data = NULL;
    ASSERT_EAARLIO_ERR(
        EAARLIO_STREAM_INVALID, stream->write(&bad, 10, (unsigned char *)&buf));
    PASS();
}

TEST test_read_invalid_seek(struct eaarlio_stream *stream)
{
    struct eaarlio_stream bad = *stream;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    bad.data = NULL;
    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_INVALID, stream->seek(&bad, 0, SEEK_SET));
    PASS();
}

TEST test_read_invalid_tell(struct eaarlio_stream *stream)
{
    struct eaarlio_stream bad = *stream;
    int64_t position;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    bad.data = NULL;
    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_INVALID, stream->tell(&bad, &position));
    PASS();
}

TEST test_read_read_null_buf(struct eaarlio_stream *stream)
{
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, stream->read(stream, 10, NULL));
    PASS();
}

TEST test_read_read_zero_len(struct eaarlio_stream *stream)
{
    unsigned char buf = 42;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_SUCCESS(stream->read(stream, 0, &buf));
    ASSERT_EQ_FMT(42, buf, "%d");
    PASS();
}

TEST test_read_read_short(struct eaarlio_stream *stream)
{
    unsigned char buf[raw_len + 5];
    unsigned char zero[5];
    memset(&buf, 0, raw_len + 5);
    memset(&zero, 0, 5);
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_READ_SHORT,
        stream->read(stream, raw_len + 5, (unsigned char *)&buf));
    ASSERT_MEM_EQ(raw, buf, raw_len);
    ASSERT_MEM_EQ(zero, buf + raw_len, 5);
    PASS();
}

TEST test_read_write_error(struct eaarlio_stream *stream)
{
    unsigned char buf[10];
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_WRITE_ERROR,
        stream->write(stream, 10, (unsigned char *)&buf));
    PASS();
}

TEST test_read_seek_invalid_whence(struct eaarlio_stream *stream)
{
    int whence = (SEEK_SET | SEEK_CUR | SEEK_END) << 1;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(
        EAARLIO_STREAM_SEEK_INVALID, stream->seek(stream, 0, whence));
    PASS();
}

TEST test_read_seek_invalid_pos(struct eaarlio_stream *stream)
{
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(
        EAARLIO_STREAM_SEEK_ERROR, stream->seek(stream, -10, SEEK_SET));
    PASS();
}

TEST test_read_tell_null_pos(struct eaarlio_stream *stream)
{
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, fn, "r"));
    ASSERT_EAARLIO_ERR(EAARLIO_NULL, stream->tell(stream, NULL));
    PASS();
}

SUITE(suite_read)
{
    struct eaarlio_stream stream;

    RUN_TEST(test_read_null_stream);

    SET_SETUP(cb_read_setup, &stream);
    SET_TEARDOWN(cb_read_teardown, &stream);

    RUN_TESTp(test_read_null_mode, &stream);
    RUN_TESTp(test_read_read_data, &stream);
    RUN_TESTp(test_read_seek_tell, &stream);

    RUN_TESTp(test_read_null_close, &stream);
    RUN_TESTp(test_read_null_read, &stream);
    RUN_TESTp(test_read_null_write, &stream);
    RUN_TESTp(test_read_null_seek, &stream);
    RUN_TESTp(test_read_null_tell, &stream);

    RUN_TESTp(test_read_invalid_close, &stream);
    RUN_TESTp(test_read_invalid_read, &stream);
    RUN_TESTp(test_read_invalid_write, &stream);
    RUN_TESTp(test_read_invalid_seek, &stream);
    RUN_TESTp(test_read_invalid_tell, &stream);

    RUN_TESTp(test_read_read_null_buf, &stream);
    RUN_TESTp(test_read_read_zero_len, &stream);
    RUN_TESTp(test_read_read_short, &stream);

    RUN_TESTp(test_read_write_error, &stream);

    RUN_TESTp(test_read_seek_invalid_whence, &stream);
    RUN_TESTp(test_read_seek_invalid_pos, &stream);

    RUN_TESTp(test_read_tell_null_pos, &stream);
}

/*******************************************************************************
 * suite_write
 *******************************************************************************
 */

struct write_data {
    char *out;
    struct eaarlio_stream stream;
};

static void cb_write_setup(void *arg)
{
    struct write_data *data = (struct write_data *)arg;
    data->stream = eaarlio_stream_empty();
    data->out = util_tempfile();
    assert(data->out);
}

static void cb_write_teardown(void *arg)
{
    struct write_data *data = (struct write_data *)arg;
    if(data->stream.data) {
        FILE *f = (FILE *)data->stream.data;
        fclose(f);
        data->stream.data = NULL;
    }
    remove(data->out);
    free(data->out);
    data->out = NULL;
}

TEST test_write_read(struct eaarlio_stream *stream, char const *out)
{
    unsigned char buf[10];
    int64_t position;

    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, out, "w"));
    ASSERT_EAARLIO_SUCCESS(stream->seek(stream, 5, SEEK_SET));
    ASSERT_EAARLIO_SUCCESS(stream->write(stream, 10, (unsigned char *)raw));
    ASSERT_EAARLIO_SUCCESS(stream->tell(stream, &position));
    ASSERT_EQ_FMT(position, 15, "%d");
    ASSERT_EAARLIO_SUCCESS(stream->close(stream));

    ASSERT_EAARLIO_SUCCESS(eaarlio_file_stream(stream, out, "r"));
    ASSERT_EAARLIO_SUCCESS(stream->seek(stream, 5, SEEK_SET));
    ASSERT_EAARLIO_SUCCESS(stream->read(stream, 10, (unsigned char *)&buf));
    ASSERT_MEM_EQ(raw, buf, 10);
    ASSERT_EAARLIO_SUCCESS(stream->close(stream));

    PASS();
}

SUITE(suite_write)
{
    struct write_data data;

    SET_SETUP(cb_write_setup, &data);
    SET_TEARDOWN(cb_write_teardown, &data);

    RUN_TESTp(test_write_read, &data.stream, data.out);
}

/*******************************************************************************
 * Run the tests
 *******************************************************************************
 */

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_null);
    RUN_SUITE(suite_read);
    RUN_SUITE(suite_write);

    GREATEST_MAIN_END();
}
