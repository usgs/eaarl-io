#ifndef EAARLIO_EDB_WRITE_H
#define EAARLIO_EDB_WRITE_H

/**
 * @file
 * @brief Write EDB data
 *
 * This header provides functionality to write EDB data to a file.
 */

#include "eaarlio/edb.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/error.h"
#include "eaarlio/stream.h"

/**
 * Write a header to an EDB file
 *
 * On success, the header is written to the file. On failure, the header may
 * have been partially written.
 *
 * @param[in] stream Stream to be written to
 * @param[in] header Header to write
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for writing.
 *
 * @post On success, the encoded data was written to @p stream.
 * @post On success, the position in @p stream will be set to the end of the
 *      header section of the file.
 * @post On failure, the encoded data may have been partially written to @p
 *      stream.
 */
eaarlio_error eaarlio_edb_write_header(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header);

/**
 * Write records to an EDB file
 *
 * @param[in] stream Stream to be written to
 * @param[in] header Header for file
 * @param[in] records Records to write
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for writing.
 *
 * @post On success, the encoded data was written to @p stream.
 * @post On success, the position in @p stream will be set to the end of the
 *      records section of the file.
 * @post On failure, the encoded data may have been partially written to @p
 *      stream.
 */
eaarlio_error eaarlio_edb_write_records(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    struct eaarlio_edb_record const *records);

/**
 * Write filenames to an EDB file
 *
 * @param[in] stream Stream to be written to
 * @param[in] header Header for file
 * @param[in] filenames Filenames to write
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for writing.
 *
 * @post On success, the encoded data was written to @p stream.
 * @post On success, the position in @p stream will be set to the end of the
 *      filenames section of the file.
 * @post On failure, the encoded data may have been partially written to @p
 *      stream.
 */
eaarlio_error eaarlio_edb_write_filenames(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    char const *const *filenames);

#endif
