#ifndef EAARLIO_EDB_READ_H
#define EAARLIO_EDB_READ_H

/**
 * @file
 * @brief Read EDB data
 *
 * This header provides functionality to read EDB data from a file.
 */

#include "eaarlio/edb.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/error.h"
#include "eaarlio/memory.h"
#include "eaarlio/stream.h"

/**
 * Read the header for an EDB file
 *
 * @param[in] stream Stream with data to read
 * @param[out] header Pointer to header to be populated
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for reading.
 *
 * @post On success, @p header is populated.
 * @post On success, the position in @p stream will be set to the end of the
 *      header.
 * @post On failure, @p header may be partially populated. Unpopulated fields
 *      are set to 0.
 */
eaarlio_error eaarlio_edb_read_header(struct eaarlio_stream *stream,
    struct eaarlio_edb_header *header);

/**
 * Read the records for an EDB file
 *
 * @param[in] stream Stream with data to read
 * @param[in] header Pointer to header data for stream
 * @param[out] records Pointer to records to be populated
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for reading.
 * @pre @p records should point to an allocated block of memory sized per @p
 *      header->record_count.
 *
 * @post On success, @p records is fully populated.
 * @post On success, the position in @p stream will be set to the end of the
 *      records.
 * @post On failure, @p records may be partially populated. Unpopulated values
 *      are not modified.
 */
eaarlio_error eaarlio_edb_read_records(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    struct eaarlio_edb_record *records);

/**
 * Read the filenames for an EDB file
 *
 * @param[in] stream Stream with data to read
 * @param[in] header Pointer to header data for stream
 * @param[out] filenames Pointer to filenames to be populated
 * @param[in] memory Memory handler, or NULL for stdlib
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for reading.
 * @pre @p *filenames should point to an allocated block of memory sized per @p
 *      header->file_count.
 *
 * @post Each non-null entry in @p **filenames points to newly allocated
 *      memory.
 * @post On success, @p filenames is fully populated: all of its pointers point
 *      to newly allocated strings where each contains a decoded filename.
 * @post On success, the position in @p stream will be set to the end of the
 *      filenames.
 * @post On failure, @p filenames may be partially populated. Entries that were
 *      not populated will be set to null.
 */
eaarlio_error eaarlio_edb_read_filenames(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    char **filenames,
    struct eaarlio_memory *memory);

#endif
