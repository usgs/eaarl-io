#ifndef EAARLIO_EDB_DECODE_H
#define EAARLIO_EDB_DECODE_H

/**
 * @file
 * @brief Low-level decoding of EDB data
 *
 * This header provides functionality to decode EDB data from buffers (arrays
 * of unsigned char). The functionality is considered low level because:
 *      - The caller is responsible for allocating any memory needed for the
 *        decoded data.
 *      - The caller is responsible for retrieving the raw data from its
 *        underlying source (typically a file).
 */

#include "eaarlio/edb.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/error.h"
#include <stdint.h>

/**
 * Decode an EDB file header
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] header Pointer to a single header value to be populated
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p header is populated.
 * @post On failure, @p header is not modified.
 */
eaarlio_error eaarlio_edb_decode_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_header *header);

/**
 * Decode an EDB record
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] record Pointer to a single record value to be populated
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p record is populated.
 * @post On failure, @p record is not modified.
 */
eaarlio_error eaarlio_edb_decode_record(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_record *record);

/**
 * Decode an EDB filename length
 *
 * Filenames are stored as length-prefixed strings. The prefix must be first
 * decoded by this function. Then you can use ::eaarlio_edb_decode_filename
 * to decode the actual filename.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] length Pointer to a single length value to be populated
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p length is populated.
 * @post On failure, @p length is not modified.
 */
eaarlio_error eaarlio_edb_decode_filename_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t *length);

/**
 * Decode an EDB filename
 *
 * Filenames are stored as length-prefixed strings. The prefix must be first
 * decoded by ::eaarlio_edb_decode_filename_length. Then you can use this
 * function to decode the actual filename.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[in] length Length of the encoded filename (as provided by its length
 *      prefix)
 * @param[out] filename Pointer to a char array to be populated
 * @param[in] filename_len Allocated size of @p filename
 *
 * @returns_eaarlio_error
 *
 * @pre @p buffer_len must be greater than or equal to @p length
 * @pre @p filename_length must be greater than @p length (ideally it should be
 *      @p length + 1 to provide space for a null terminator)
 *
 * @post On success, @p filename is populated.
 * @post On failure, @p filename is not modified.
 */
eaarlio_error eaarlio_edb_decode_filename(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t length,
    char *filename,
    size_t filename_len);

#endif
