#include "eaarlio/error.h"
#include "eaarlio/file.h"
#include "eaarlio/stream.h"
#include "eaarlio/tld_opener.h"
#include "greatest.h"
#include "assert_error.h"
#include "mock_memory.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

TEST test_sanity()
{
    eaarlio_file_tld_opener(NULL, NULL, NULL);
    PASS();
}

TEST test_null_opener()
{
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_file_tld_opener(NULL, "test", NULL));
    PASS();
}

TEST test_null_path()
{
    struct eaarlio_tld_opener opener;
    ASSERT_EAARLIO_ERR(
        EAARLIO_NULL, eaarlio_file_tld_opener(&opener, NULL, NULL));
    PASS();
}

SUITE(suite_basic)
{
    RUN_TEST(test_sanity);
    RUN_TEST(test_null_opener);
    RUN_TEST(test_null_path);
}

TEST test_open(struct eaarlio_memory *memory)
{
    struct eaarlio_tld_opener opener;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_tld_opener(&opener, DATADIR, memory));
    PASS();
}

TEST test_open_close(struct eaarlio_memory *memory, struct mock_memory *mock)
{
    struct eaarlio_tld_opener opener;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_tld_opener(&opener, DATADIR, memory));
    ASSERT_EAARLIO_SUCCESS(opener.close(&opener));
    ASSERT_EQ_FMT(0, mock_memory_count_in_use(mock), "%d");
    PASS();
}

SUITE(suite_memory)
{
    struct mock_memory mock;
    struct eaarlio_memory memory;

    mock_memory_new(&memory, &mock, 10);
    RUN_TESTp(test_open, &memory);

    mock_memory_reset(&mock, 10);
    RUN_TESTp(test_open_close, &memory, &mock);

    mock_memory_destroy(&memory);
}

TEST test_open_tld_exists(struct eaarlio_memory *memory,
    struct mock_memory *mock,
    struct eaarlio_stream *stream)
{
    struct eaarlio_tld_opener opener;
    int64_t position;
    unsigned char buf[5];

    ASSERT_EAARLIO_SUCCESS(eaarlio_file_tld_opener(&opener, DATADIR, memory));
    ASSERT_EAARLIO_SUCCESS(opener.open_tld(&opener, stream, "alphanum.txt"));

    ASSERT_EAARLIO_SUCCESS(stream->tell(stream, &position));
    ASSERT_EQ_FMT(0, position, "%d");

    ASSERT_EAARLIO_SUCCESS(stream->read(stream, 5, (unsigned char *)buf));
    ASSERT_MEM_EQ("abcde", buf, 5);

    ASSERT_EAARLIO_SUCCESS(stream->close(stream));
    ASSERT_EAARLIO_SUCCESS(opener.close(&opener));
    ASSERT_EQ_FMT(0, mock_memory_count_in_use(mock), "%d");
    PASS();
}

TEST test_open_tld_missing(struct eaarlio_memory *memory,
    struct mock_memory *mock,
    struct eaarlio_stream *stream)
{
    struct eaarlio_tld_opener opener;
    ASSERT_EAARLIO_SUCCESS(eaarlio_file_tld_opener(&opener, DATADIR, memory));
    ASSERT_EAARLIO_ERR(EAARLIO_STREAM_OPEN_ERROR,
        opener.open_tld(&opener, stream, "missing.txt"));
    ASSERT_EAARLIO_SUCCESS(opener.close(&opener));
    ASSERT_EQ_FMT(0, mock_memory_count_in_use(mock), "%d");
    PASS();
}

SUITE(suite_stream)
{
    struct mock_memory mock;
    struct eaarlio_memory memory;
    struct eaarlio_stream stream = eaarlio_stream_empty();

    mock_memory_new(&memory, &mock, 10);
    RUN_TESTp(test_open_tld_exists, &memory, &mock, &stream);
    if(stream.close)
        stream.close(&stream);

    stream = eaarlio_stream_empty();
    mock_memory_reset(&mock, 10);
    RUN_TESTp(test_open_tld_missing, &memory, &mock, &stream);
    if(stream.close)
        stream.close(&stream);

    mock_memory_destroy(&memory);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(suite_basic);
    RUN_SUITE(suite_memory);
    RUN_SUITE(suite_stream);

    GREATEST_MAIN_END();
}
