#ifndef EAARLIO_EDB_H
#define EAARLIO_EDB_H

/**
 * @file
 * @brief Constants and structures for the EDB file format
 *
 * EDB stands for "EAARL database". The files are also sometimes called "index
 * files" and have the corresponding file extension ".idx". The EDB file serves
 * as an index into a set of raw lidar TLD files. The raw lidar TLD records do
 * not have a fixed size, so the EDB file provides the information necessary to
 * support random-access retrieval of TLD records.
 *
 * The records in the EDB also define the raster numbers for the data. Although
 * the TLD records contain a raster field, it is not used as it cycles.
 * Instead, the one-based index of a record in the EDB is defined as its raster
 * number. For example, the TLD record that corresponds with the 20th record in
 * the EDB is defined to be raster 20.
 *
 * The EDB file consists of an EDB header, followed by a series of EDB records,
 * followed by a series of length-prefixed file names.
 */

#include "eaarlio/error.h"
#include "eaarlio/memory.h"
#include "eaarlio/stream.h"
#include <stdint.h>

/******************************************************************************/

/**
 * EDB record
 *
 * An EDB file contains one record for each raster in its associated set of TLD
 * files.
 */
struct eaarlio_edb_record {
    /**
     * Integer seconds component of timestamp
     *
     * Use ::eaarlio_units_edb_time to derive floating point time
     */
    uint32_t time_seconds;

    /**
     * Fractional seconds component of timestamp
     *
     * Use ::eaarlio_units_edb_time to derive floating point time
     */
    uint32_t time_fraction;

    /** Byte offset in TLD file to this record */
    uint32_t record_offset;

    /** Byte length of this record (including the header) */
    uint32_t record_length;

    /** Index into ::eaarlio_edb::files for this record's TLD file */
    int16_t file_index;

    /** Number of pulses in this raster */
    uint8_t pulse_count;

    /** Digitizer used: 0 or 1 */
    uint8_t digitizer;
};

/**
 * Empty eaarlio_edb_record value
 *
 * All fields will contain zero values.
 */
#define eaarlio_edb_record_empty()                                             \
    (struct eaarlio_edb_record)                                                \
    {                                                                          \
        0, 0, 0, 0, 0, 0, 0                                                    \
    }

/**
 * EDB index data
 *
 * This structure represents the data from an EDB file. Correspondingly, it's
 * the in-memory representation of the index.
 */
struct eaarlio_edb {
    /** Number of records in eaarlio_edb::records */
    uint32_t record_count;

    /** Number of files in eaarlio_edb::files */
    uint32_t file_count;

    /** Record entries for the TLD rasters */
    struct eaarlio_edb_record *records;

    /** Names of the TLD files */
    char **files;
};

/**
 * Empty ::eaarlio_edb value
 *
 * All numeric fields will contain zero values. All pointers will be @c NULL.
 */
#define eaarlio_edb_empty()                                                    \
    (struct eaarlio_edb)                                                       \
    {                                                                          \
        0, 0, NULL, NULL                                                       \
    }

/**
 * Free memory allocated to fields in ::eaarlio_edb
 *
 * On success:
 * @param[in,out] edb EDB with memory to release
 * @param[in] memory Memory handler, or NULL for stdlib
 *
 * @returns_eaarlio_error
 *
 * @post On success, edb->records and edb->files will be null.
 * @post On success, any memory that was allocated for edb->records or
 *      edb->files has been released.
 *
 * @remark The pointer to @p edb is not released.
 * @remark Pointer fields may be @c NULL. If so, they are left alone.
 */
eaarlio_error eaarlio_edb_free(struct eaarlio_edb *edb,
    struct eaarlio_memory *memory);

/**
 * Read an entire EDB file
 *
 * @param[in] stream Stream with data to read
 * @param[out] edb Pointer to EDB index to be populated
 * @param[in] memory Memory handler, or NULL for stdlib
 * @param[in] include_records Should record data be read? 1 = yes, 0 = no
 * @param[in] include_files Should TLD filenames be read? 1 = yes, 0 = no
 *
 * @returns_eaarlio_error
 *
 * @post If @p edb->records is not null, then it points to newly allocated
 *      memory.
 * @post If @p edb->filenames is not null, then it points to newly allocated
 *      memory.
 * @post If @p edb->filenames is not null, then each non-null entry in it
 *      points to newly allocated memory.
 * @post On success:
 * - @p edb->record_count and @p edb->file_count are populated.
 * - if include_records = 1, then @p edb->records will be populated.
 * - if include_records = 0, then @p edb->records will be @c NULL.
 * - if include_files = 1, then @p edb->files will be populated.
 * - if include_files = 0, then @p edb->files will be @c NULL.
 * @post On failure, @p edb->records and @p edb->files may be partially
 *      populated. Anything not populated will be null.
 */
eaarlio_error eaarlio_edb_read(struct eaarlio_stream *stream,
    struct eaarlio_edb *edb,
    struct eaarlio_memory *memory,
    int include_records,
    int include_files);

/**
 * Write an EDB file
 *
 * @param[in] stream Stream to be written to
 * @param[in] edb Pointer to EDB index to write
 *
 * @returns_eaarlio_error
 *
 * @post On success, the encoded data was written to @p stream.
 */
eaarlio_error eaarlio_edb_write(struct eaarlio_stream *stream,
    struct eaarlio_edb const *edb);

#endif
