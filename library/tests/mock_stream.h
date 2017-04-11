#ifndef MOCK_STREAM_H
#define MOCK_STREAM_H

#include "eaarlio/error.h"
#include "eaarlio/stream.h"
#include "greatest_extend.h"
#include <assert.h>
#include <stdint.h>

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

struct mock_stream {
    /* Value to return for size. May be extended if do_write is enabled. */
    int64_t size;

    /* Maximum size permitted. This much memory is allocated for data. */
    int64_t size_max;

    /* Pointer to memory allocated for the mock file content */
    unsigned char *data;

    /* Current location in file */
    int64_t offset;

    /* If sequential = 1, then attempting to seek backwards fails */
    int sequential;

    /* Disable read/write (for write-only or read-only streams) */
    int no_read, no_write;
};

/* ----- Mock management
 * Functions for creating/destroying/managing mock streams
 */

/* Create a new eaarlio_stream and initialize for mocking.
 */
struct eaarlio_stream *mock_stream_stream_new(struct mock_stream *m);

/* Releases memory for the eaarlio_stream created by mock_stream_stream_new.
 *
 * This does NOT free the memory associated with the mock_stream referenced by
 * s->data.
 */
void mock_stream_stream_destroy(struct eaarlio_stream *s);

/* Create a new mock_stream.
 *
 * This allocates memory for the mock_stream, as well as for its sub-elements.
 * It also initializes fields to sane values.
 *
 * Parameter:
 *      size_max - The maximum permitted size for stored data.
 */
struct mock_stream *mock_stream_new(int64_t size_max);

/* Free memory associated with a mock.
 *
 * This must be called on any mock_stream once finished. Do not call free
 * directly or you will create a memory leak.
 *
 * The pointer should not be used afterwards.
 */
void mock_stream_destroy(struct mock_stream *mock);

/* ----- Check functions
 */

/**
 * Lowest address written to
 */
int64_t mock_stream_data_write_low(struct mock_stream *mock);

/**
 * Highest address written to
 */
int64_t mock_stream_data_write_high(struct mock_stream *mock);

/**
 * Write given buffer to mock at current offset
 *
 * Advances offset after
 */
void mock_stream_write(struct mock_stream *mock,
    int64_t len,
    unsigned char const *buffer);

/**
 * Verify that mock matches expected at current offset
 *
 * Advances offset after
 */
TEST_TYPE mock_stream_verify(unsigned char const *expected,
    struct mock_stream *mock,
    int64_t len);

/**
 * As mock_stream_verify but includes a message
 */
TEST_TYPE mock_stream_verify_msg(char const *msg,
    unsigned char const *expected,
    struct mock_stream *mock,
    int64_t len);

/**
 * Verify that all values in mock at offset for len equal expected
 *
 * Advances offset after
 */
TEST_TYPE mock_stream_verify_fixed(unsigned char expected,
    struct mock_stream *mock,
    int64_t len);

/**
 * As mock_stream_verify_fixed but includes a message
 */
TEST_TYPE mock_stream_verify_fixed_msg(char const *msg,
    unsigned char expected,
    struct mock_stream *mock,
    int64_t len);

#endif
