#ifndef EAARLIO_STREAM_H
#define EAARLIO_STREAM_H

/**
 * @file
 * @brief Stream interface for working with files
 *
 * This header defines an interface for working with files and other file-like
 * streams. This decouples the library from direct file access so that other
 * kinds of streams can be implemented if needed, such as compressed streams.
 */

#include "eaarlio/error.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * Stream interface
 *
 * This interface generalizes the operations needed to read from and write to a
 * file. The intent is to make it possible to read from streams other than a
 * "normal" file: for example, a compressed file.
 *
 * Each function accepts as its first parameter a pointer to the
 * ::eaarlio_stream. This gives the functions access to the stream's @c data
 * field, which is a pointer to whatever the implementation may need for
 * internal state.
 *
 * All functions return an ::eaarlio_error and should use ::EAARLIO_SUCCESS to
 * represet a successful outcome. If it is not plausible to provide an
 * implementation for a given function, then it should return
 * ::EAARLIO_STREAM_NOT_IMPL. Each function is documented with some suggested
 * error codes for other failing conditions, but you are not restricted to
 * those listed. You can return any valid ::eaarlio_error value as needed.
 *
 * This interface is intended to support random-access reads and writes. Not
 * all streams are suitable for random-access. In such cases, your stream
 * should pretend to be random-access as much as is possible. For example,
 * requests to seek should succeed if they're to a current position; requests
 * to seek for a future position should simply discard data to that point. If
 * possible, consider implementing random read access to permit backtracking by
 * restarting the stream from the beginning as a last resort. Except where
 * noted, library functions operate sequentially so backtracking shouldn't
 * cause a significant performance penalty internally.
 */
struct eaarlio_stream {
    /**
     * Close the stream
     *
     * @param[in,out] self A pointer to the stream
     *
     * @returns Any ::eaarlio_error code. Recommended values are given
     *      below.
     * @retval ::EAARLIO_SUCCESS on success
     * @retval ::EAARLIO_NULL if provided a @c NULL pointer
     * @retval ::EAARLIO_STREAM_INVALID if @p stream is not valid
     * @retval ::EAARLIO_STREAM_CLOSE_ERROR if an error is encountered while
     *      closing
     * @retval ::EAARLIO_STREAM_NOT_IMPL if this function isn't implemented
     *
     * @post On success, all fields of ::eaarlio_stream must be set to @c
     *      NULL.
     */
    eaarlio_error (*close)(struct eaarlio_stream *self);

    /**
     * Read bytes from a stream and place them in a buffer
     *
     * @param[in] self A pointer to the stream
     * @param[in] len The number of bytes to read
     * @param[out] buffer The destination for the bytes read
     *
     * @returns Any ::eaarlio_error code. Recommended values are given
     *      below.
     * @retval ::EAARLIO_SUCCESS on success
     * @retval ::EAARLIO_NULL if provided a @c NULL pointer
     * @retval ::EAARLIO_STREAM_INVALID if @p stream is not valid
     * @retval ::EAARLIO_STREAM_READ_SHORT if the read request returned fewer
     *      bytes than requested (this is generally interpreted as being at the
     *      end of the file)
     * @retval ::EAARLIO_STREAM_READ_ERROR if an error is encountered during
     *      the read
     * @retval ::EAARLIO_STREAM_NOT_IMPL if this function isn't implemented
     */
    eaarlio_error (*read)(struct eaarlio_stream *self,
        uint64_t len,
        unsigned char *buffer);

    /**
     * Writes bytes from a buffer to a stream
     *
     * @param[in] self A pointer to the stream
     * @param[in] len The number of bytes to write
     * @param[in] buffer The data to write
     *
     * @returns Any ::eaarlio_error code. Recommended values are given
     *      below.
     * @retval ::EAARLIO_SUCCESS on success
     * @retval ::EAARLIO_NULL if provided a @c NULL pointer
     * @retval ::EAARLIO_STREAM_INVALID if @p stream is not valid
     * @retval ::EAARLIO_STREAM_WRITE_SHORT if the write request wrote fewer
     *      bytes than requested
     * @retval ::EAARLIO_STREAM_WRITE_ERROR if an error is encountered during
     *      the write
     * @retval ::EAARLIO_STREAM_NOT_IMPL if this function isn't implemented
     */
    eaarlio_error (*write)(struct eaarlio_stream *self,
        uint64_t len,
        unsigned char const *buffer);

    /**
     * Set the current position for the stream
     *
     * The position in the stream will be set based on the given values for @p
     * offset and @p whence, possibly relative to the current stream position.
     * The position is set based on the value of @p whence, using the same
     * meaning as for the C standard library function fseek:
     *
     * | whence      | Interpretation                                |
     * | ----------- | --------------------------------------------- |
     * | @c SEEK_SET | @p offset is from the start of the file       |
     * | @c SEEK_CUR | @p offset is relative to the current position |
     * | @c SEEK_END | @p offset is from the end of the file         |
     *
     * @note The library only requires @c SEEK_SET and @c SEEK_CUR. Your
     *      implementation can respond to @c SEEK_END with
     *      ::EAARLIO_STREAM_SEEK_INVALID if appropriate.
     *
     * @param[in] self A pointer to the stream
     * @param[in] offset Offset to apply, as indicated by @p whence
     * @param[in] whence How to apply @p offset to the current position. This
     *      should be one of SEEK_SET, SEEK_CUR, or SEEK_END.
     *
     * @returns Any ::eaarlio_error code. Recommended values are given
     *      below.
     * @retval ::EAARLIO_SUCCESS on success
     * @retval ::EAARLIO_NULL if provided a @c NULL pointer
     * @retval ::EAARLIO_STREAM_INVALID if @p stream is not valid
     * @retval ::EAARLIO_STREAM_SEEK_INVALID if an invalid value is provided for
     *      @p whence
     * @retval ::EAARLIO_STREAM_SEEK_ERROR if an error is encountered during the
     *      seek
     * @retval ::EAARLIO_STREAM_NOT_IMPL if this function isn't implemented
     */
    eaarlio_error (
        *seek)(struct eaarlio_stream *self, int64_t offset, int whence);

    /**
     * Retrieve the current position in the stream
     *
     * @param[in] self A pointer to the stream
     * @param[out] position Pointer to single position value to be populated
     *
     * @returns Any ::eaarlio_error code. Recommended values are given
     *      below.
     * @retval ::EAARLIO_SUCCESS on success
     * @retval ::EAARLIO_NULL if provided a @c NULL pointer
     * @retval ::EAARLIO_STREAM_INVALID if @p stream is not valid
     * @retval ::EAARLIO_STREAM_TELL_ERROR if an error is encountered during the
     *      tell
     * @retval ::EAARLIO_STREAM_NOT_IMPL if this function isn't implemented
     */
    eaarlio_error (*tell)(struct eaarlio_stream *self, int64_t *position);

    /**
     * Internal data pointer
     *
     * This is not used by calling code directly. It provides a means for the
     * functions in the structure to maintain internal state.
     */
    void *data;
};

/**
 * Empty eaarlio_stream value
 *
 * All pointers will be @c NULL.
 */
#define eaarlio_stream_empty()                                                 \
    (struct eaarlio_stream)                                                    \
    {                                                                          \
        NULL, NULL, NULL, NULL, NULL, NULL                                     \
    }

#endif
