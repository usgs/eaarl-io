#ifndef EAARLIO_EDB_ENCODE_H
#define EAARLIO_EDB_ENCODE_H

/**
 * @file
 * @brief Low-level encoding of EDB data
 *
 * This header provides functionality to encode EDB to buffers (arrays of
 * unsigned char). The functionality is considered low level because:
 *      - The caller is responsible for allocating any memory needed for the
 *        buffers.
 *      - The caller is responsible for storing the encoded data to whatever
 *        underlying destination is desired (typically a file), if applicable.
 */

#include "eaarlio/edb.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/error.h"
#include <stdint.h>

/**
 * Encode an EDB file header
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Allocated size of @p buffer
 * @param[in] header Pointer to single header value to be encoded
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p buffer is populated.
 * @post On failure, @p buffer is not modified.
 */
eaarlio_error eaarlio_edb_encode_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_header const *header);

/**
 * Encode an EDB record
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Allocated size of @p buffer
 * @param[in] record Pointer to single record value to be encoded
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p buffer is populated.
 * @post On failure, @p buffer is not modified.
 */
eaarlio_error eaarlio_edb_encode_record(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_record const *record);

/**
 * Encode a filename's length
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Allocated size of @p buffer
 * @param[in] length Length value to be encoded
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p buffer is populated.
 * @post On failure, @p buffer is not modified.
 */
eaarlio_error eaarlio_edb_encode_filename_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint16_t length);

/**
 * Encode a filename
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Allocated size of @p buffer
 * @param[in] filename Null-terminated filename value to be encoded
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p buffer is populated.
 * @post On failure, @p buffer is not modified.
 */
eaarlio_error eaarlio_edb_encode_filename(unsigned char *buffer,
    uint32_t buffer_len,
    char const *filename);

#endif
