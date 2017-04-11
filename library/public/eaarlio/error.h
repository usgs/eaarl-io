#ifndef EAARLIO_ERROR_H
#define EAARLIO_ERROR_H

/**
 * @file
 * @brief Error codes and handling
 *
 * This header defines the error codes that are used as return results
 * throughout the library. It also provides two functions that can be used to
 * provide human-readable information about error codes.
 */

#include <stdio.h>
#include <string.h>

typedef enum {

    /* -- General use */

    /** Success (no error was encountered) */
    EAARLIO_SUCCESS,
    /** An unexpected null pointer was encountered */
    EAARLIO_NULL,
    /** A buffer's allocated size was too small */
    EAARLIO_BUFFER_SHORT,
    /** A value was encountered that was out of range for its type */
    EAARLIO_VALUE_OUT_OF_RANGE,
    /** Unable to complete an operation due to suspected corrupted data */
    EAARLIO_CORRUPT,
    /** Encountered a string that appeared unterminated */
    EAARLIO_STRING_UNTERMINATED,

    /* -- Specific to flight */

    /** Invalid flight configuration */
    EAARLIO_FLIGHT_INVALID,
    /** An invalid raster number was specified */
    EAARLIO_FLIGHT_RASTER_INVALID,

    /* -- Specific to edb */

    /** Encountered a filename whose length exceeds what EDB supports */
    EAARLIO_EDB_FILENAME_TOO_LONG,

    /* -- Specific to tld */

    /** Encountered an unknown TLD record type */
    EAARLIO_TLD_TYPE_UNKNOWN,
    /** Invalid TLD opener */
    EAARLIO_TLD_OPENER_INVALID,

    /* -- Specific to memory */

    /** Invalid memory handler */
    EAARLIO_MEMORY_INVALID,
    /** Unable to allocate memory */
    EAARLIO_MEMORY_ALLOC_FAIL,

    /* -- Specific to stream */

    /** Invalid stream configuration */
    EAARLIO_STREAM_INVALID,
    /** Attempted to use a stream operation that was not implemented */
    EAARLIO_STREAM_NOT_IMPL,
    /** Unable to open file */
    EAARLIO_STREAM_OPEN_ERROR,
    /** Unable to close file */
    EAARLIO_STREAM_CLOSE_ERROR,
    /** Attempt to read failed */
    EAARLIO_STREAM_READ_ERROR,
    /** Fewer bytes read than requested (may signal end of file) */
    EAARLIO_STREAM_READ_SHORT,
    /** Attempt to write failed */
    EAARLIO_STREAM_WRITE_ERROR,
    /** Fewer bytes written than expected */
    EAARLIO_STREAM_WRITE_SHORT,
    /** Unable to seek to position in file */
    EAARLIO_STREAM_SEEK_ERROR,
    /** Request to seek used invalid whence */
    EAARLIO_STREAM_SEEK_INVALID,
    /** Unable to tell position in file */
    EAARLIO_STREAM_TELL_ERROR,

    /**
     * Unknown error
     *
     * This is not intended to be explicitly used. It primarily exists as a
     * terminal value in the enum to support looping in test cases. If none of
     * the existing enum values work for a new error situation, then a new
     * value should be added to the enum.
     */
    EAARLIO_ERROR_UNKNOWN
} eaarlio_error;

/**
 * Retrieve the symbolic name for an error
 *
 * For example, `eaarlio_error_name(EAARLIO_SUCCESS)` will return
 * `"EAARLIO_SUCCESS"`.
 *
 * @param[in] err An error code
 *
 * @returns A string containing the name of the error code
 */
char const *eaarlio_error_name(eaarlio_error err);

/**
 * Retrieve an error message for an error
 *
 * For example, `eaarlio_error_message(EAARLIO_SUCCESS)` will return
 * `"Success!"`.
 *
 * @param[in] err An error code
 *
 * @returns A string containing a short message that describes the error code
 */
char const *eaarlio_error_message(eaarlio_error err);

/**
 * Check for errors and display error if needed
 *
 * This is a simple handler for checking eaarlio_error codes and displaying
 * error messages.
 *
 * When @p err is ::EAARLIO_SUCCESS, this function does nothing except return 0.
 *
 * When @p err is anything other than ::EAARLIO_SUCCESS, it prints an error
 * message to stderr and returns 1. If @p format is provided, then fprintf will
 * be used to print the supplied message (passing any additional parameters
 * through to it). Then ::eaarlio_error_name and ::eaarlio_error_message
 * are used to display the error code and a short description to stderr.
 *
 * @p format can be @c null, in which case just the error code and description
 * are output to stderr.
 *
 * @param[in] err Error code from an eaarlio library function
 * @param[in] format Format string for message to print on error
 * @param[in] ... Additional arguments to use when printing error
 *
 * @retval 0 if @p err is ::EAARLIO_SUCCESS
 * @retval 1 if @p err is not ::EAARLIO_SUCCESS
 *
 * @post If @p err is not ::EAARLIO_SUCCESS, then an error message is printed to
 *      stderr.
 *
 * As an example, consider this sample code:
 *
 * @code
 * eaarlio_error err = EAARLIO_NULL;
 * int raster_number = 42;
 * int failed = eaarlio_error_check(err,
 *      "Failed to access raster %d", raster_number);
 * @endcode
 *
 * Because @c err is not ::EAARLIO_SUCCESS, this would print the following
 * message to stderr and assign 1 to @c failed:
 *
 * @code{plaintext}
 * Failed to access raster 42
 * Error code: EAARLIO_NULL: Attempted to use a null pointer
 * @endcode
 */
int eaarlio_error_check(eaarlio_error err, char *format, ...);

#endif
