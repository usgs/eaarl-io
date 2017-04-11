#ifndef EAARLIO_FILE_H
#define EAARLIO_FILE_H

/**
 * @file
 * @brief File-based implementations for interfaces
 *
 * The library has three generalized interfaces to decouple it from direct file
 * access. This header provides implementations of those interfaces for working
 * with normal files.
 */

#include "eaarlio/error.h"
#include "eaarlio/flight.h"
#include "eaarlio/memory.h"
#include "eaarlio/stream.h"
#include "eaarlio/tld_opener.h"

/**
 * Open an eaarlio_flight using normal files
 *
 * @param[out] flight Pointer to flight to be populated
 * @param[in] edb_file Path to the EDB file to load
 * @param[in] tld_path Path where the associated TLD files are located
 * @param[in] memory Memory handler, or NULL for stdlib
 *
 * @returns_eaarlio_error
 *
 * Anything from eaarlio_file_stream
 * Anything from eaarlio_edb_read
 * Anything from stream.close
 * Anything from eaarlio_file_tld_opener
 * Anything from eaarlio_flight_init
 */
eaarlio_error eaarlio_file_flight(struct eaarlio_flight *flight,
    char const *edb_file,
    char const *tld_path,
    struct eaarlio_memory *memory);

/**
 * Open a stream for a normal file
 *
 * This opens a binary file and configures @p stream to access it using the C
 * standard library file functionality (<stdio.h>).
 *
 * Parameters @p fn and @p mode are passed directly to fopen.
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_file_stream(
    /** Stream to open the file with */
    struct eaarlio_stream *stream,
    /** Path to file to open */
    char const *fn,
    /** Mode to open the file as */
    char const *mode);

/**
 * Open a tld_opener using normal files
 */
eaarlio_error eaarlio_file_tld_opener(struct eaarlio_tld_opener *tld_opener,
    char const *tld_path,
    struct eaarlio_memory *memory);

#endif
