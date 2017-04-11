#include "mock_stream.h"
#include "eaarlio/error.h"
#include "eaarlio/stream.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

static eaarlio_error _mock_read(struct eaarlio_stream *self,
    uint64_t len,
    unsigned char *buf)
{
    struct mock_stream *mock = (struct mock_stream *)self->data;

    if(mock->no_read)
        return EAARLIO_STREAM_READ_ERROR;

    /* Reduce len if needed to ensure the read does not exceed allocated
     * storage.
     */
    if(mock->offset + len > (uint64_t)mock->size_max) {
        if(mock->offset > mock->size_max) {
            len = 0;
        } else {
            len = mock->size_max - mock->offset;
        }
    }

    if(len) {
        assert(len <= SIZE_MAX);
        memcpy(buf, &mock->data[mock->offset], (size_t)len);
        mock->offset += len;
    }

    return EAARLIO_SUCCESS;
}

static eaarlio_error _mock_write(struct eaarlio_stream *self,
    uint64_t len,
    unsigned char const *buf)
{
    struct mock_stream *mock = (struct mock_stream *)self->data;

    if(mock->no_write)
        return EAARLIO_STREAM_WRITE_ERROR;

    /* Reduce len if needed to ensure the write does not exceed allocated
     * storage.
     */
    if(mock->offset + len > (uint64_t)mock->size_max) {
        if(mock->offset > mock->size_max) {
            len = 0;
        } else {
            len = mock->size_max - mock->offset;
        }
    }

    if(len) {
        assert(len <= SIZE_MAX);
        memcpy(&mock->data[mock->offset], buf, (size_t)len);

        if(mock->offset + len > (uint64_t)mock->size)
            mock->size = mock->offset + len;

        mock->offset += len;
    }

    return EAARLIO_SUCCESS;
}

static eaarlio_error _mock_seek(struct eaarlio_stream *self,
    int64_t offset,
    int whence)
{
    struct mock_stream *mock = (struct mock_stream *)self->data;
    int64_t new_offset;

    switch(whence) {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset = mock->offset + offset;
            break;
        /* Disallow SEEK_END since not all streams can support it */
        case SEEK_END:
            return EAARLIO_STREAM_SEEK_ERROR;
        default:
            return EAARLIO_STREAM_SEEK_INVALID;
    }

    if(mock->sequential && new_offset < mock->offset) {
        return EAARLIO_STREAM_SEEK_ERROR;
    }
    mock->offset = new_offset;

    return EAARLIO_SUCCESS;
}

static eaarlio_error _mock_tell(struct eaarlio_stream *self, int64_t *offset)
{
    struct mock_stream *mock = (struct mock_stream *)self->data;
    *offset = mock->offset;
    return EAARLIO_SUCCESS;
}

static eaarlio_error _mock_close(struct eaarlio_stream *self)
{
    struct mock_stream *mock = (struct mock_stream *)self->data;
    if(mock->data)
        free(mock->data);
    free(mock);
    *self = eaarlio_stream_empty();
    return EAARLIO_SUCCESS;
}

struct eaarlio_stream *mock_stream_stream_new(struct mock_stream *m)
{
    struct eaarlio_stream *s = calloc(1, sizeof(struct eaarlio_stream));
    assert(s);

    s->close = &_mock_close;
    s->read = &_mock_read;
    s->write = &_mock_write;
    s->seek = &_mock_seek;
    s->tell = &_mock_tell;
    s->data = m;

    return s;
}

void mock_stream_stream_destroy(struct eaarlio_stream *s)
{
    free(s);
}

struct mock_stream *mock_stream_new(int64_t size_max)
{
    struct mock_stream *mock = calloc(1, sizeof(struct mock_stream));
    assert(mock);

    mock->size_max = size_max;
    if(size_max > 0) {
        assert((uint64_t)size_max <= SIZE_MAX);
        mock->data = calloc(1, (size_t)size_max);
        assert(mock->data);
    } else {
        size_max = 0;
    }

    mock->size = size_max;

    return mock;
}

void mock_stream_destroy(struct mock_stream *mock)
{
    if(mock->data)
        free(mock->data);
    free(mock);
}

void mock_stream_write(struct mock_stream *mock,
    int64_t len,
    unsigned char const *buffer)
{
    assert(mock->offset + len <= mock->size_max);
    memcpy(&mock->data[mock->offset], buffer, len);
    mock->offset += len;
    if(mock->offset > mock->size)
        mock->size = mock->offset;
}

TEST_TYPE mock_stream_verify(unsigned char const *expected,
    struct mock_stream *mock,
    int64_t len)
{
    assert(mock->offset + len <= mock->size_max);
    ASSERT_MEM_EQ(expected, &mock->data[mock->offset], len);
    mock->offset += len;
    PASS();
}

TEST_TYPE mock_stream_verify_msg(char const *msg,
    unsigned char const *expected,
    struct mock_stream *mock,
    int64_t len)
{
    assert(mock->offset + len <= mock->size_max);
    ASSERT_MEM_EQm(msg, expected, &mock->data[mock->offset], len);
    mock->offset += len;
    PASS();
}

TEST_TYPE mock_stream_verify_fixed(unsigned char expected,
    struct mock_stream *mock,
    int64_t len)
{
    assert(len > 0);
    unsigned char *buf = malloc(len * sizeof(unsigned char));
    assert(buf);
    memset(buf, expected, len);
    TEST_TYPE result = mock_stream_verify(buf, mock, len);
    free(buf);
    CHECK_CALL(result);
    PASS();
}

TEST_TYPE mock_stream_verify_fixed_msg(char const *msg,
    unsigned char expected,
    struct mock_stream *mock,
    int64_t len)
{
    assert(len > 0);
    unsigned char *buf = malloc(len * sizeof(unsigned char));
    assert(buf);
    memset(buf, expected, len);
    TEST_TYPE result = mock_stream_verify_msg(msg, buf, mock, len);
    free(buf);
    CHECK_CALL(result);
    PASS();
}
