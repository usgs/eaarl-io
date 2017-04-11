#ifndef EAARLIO_EDB_INTERNALS_H
#define EAARLIO_EDB_INTERNALS_H

/**
 * @file
 * @brief Internals for EDB file format: constants and structs
 */

#include <stdint.h>

/**
 * Encoded byte size of an EDB header
 */
#define EAARLIO_EDB_HEADER_SIZE 12U

/**
 * Encoded byte size of an EDB record
 */
#define EAARLIO_EDB_RECORD_SIZE 20U

/**
 * Encoded byte size of the length prefix for a filename in an EDB file
 */
#define EAARLIO_EDB_FILENAME_LENGTH_SIZE 2U

/**
 * Maximum size for a filename in an EDB file
 */
#define EAARLIO_EDB_FILENAME_MAX_LENGTH                                        \
    (1 << (EAARLIO_EDB_FILENAME_LENGTH_SIZE * 8))

/**
 * EDB file header
 *
 * The EDB file format starts with one of these.
 */
struct eaarlio_edb_header {
    /** Offset to the start of the files data */
    uint32_t files_offset;

    /** Number of index records */
    uint32_t record_count;

    /** Number of TLD files */
    uint32_t file_count;
};

#endif
