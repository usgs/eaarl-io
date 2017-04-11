#ifndef EAARLIO_TLD_OPENER_H
#define EAARLIO_TLD_OPENER_H

/**
 * @file
 * @brief Interface for opening TLD files
 *
 * This header defines an interface for opening TLD files for reading. It is
 * intended to permit a TLD file to be opened using just the file name provided
 * from the EDB file. The TLD opener is responsible for resolving the file's
 * actual location in the filesystem and is responsible for opening an
 * appropriate stream for it.
 */

#include "eaarlio/error.h"
#include "eaarlio/stream.h"

/**
 * TLD file opener interface
 *
 * This interface generalizes the operations needed to open a TLD file in read
 * mode given its file name as provided by an EDB file. This interface
 * addresses several issues:
 * - The TLD file name in the EDB does not have any path information. The TLD
 *   opener is responsible for knowing where to look for the file given just
 *   its name.
 * - The TLD opener is also responsible for knowing how to open a stream for
 *   that file.
 *
 * Both functions return an ::eaarlio_error and should use ::EAARLIO_SUCCESS to
 * represent a successful outcome. Each function is documented with some
 * suggested error codes for failing conditions, but you are not restricted to
 * those listed. In particular, stream-related errors should be propagated to
 * the caller as-is.
 */
struct eaarlio_tld_opener {
    /**
     * Open a readable stream for a TLD file
     *
     * @param[in] self The TLD opener being used
     * @param[out] stream The stream to be opened
     * @param[in] tld_file The name of the TLD file to open
     *
     * @retval ::EAARLIO_SUCCESS on success
     * @retval ::EAARLIO_NULL if provided a null pointer
     * @retval ::EAARLIO_TLD_OPENER_INVALID if the TLD opener is not valid
     *
     * @post On success, @p stream must be a valid, readable stream for the
     *      requested TLD file.
     */
    eaarlio_error (*open_tld)(struct eaarlio_tld_opener *self,
        struct eaarlio_stream *stream,
        char const *tld_file);

    /**
     * Close the opener
     *
     * @param[in] self The TLD opener to be closed
     *
     * @retval ::EAARLIO_SUCCESS on success
     * @retval ::EAARLIO_NULL if provided a null pointer
     * @retval ::EAARLIO_TLD_OPENER_INVALID if the TLD opener is not valid
     *
     * @post On success, any internal resources allocated by the implementation
     *      must be released.
     * @post On success, @p self should have all of its pointers set to @c
     *      NULL, as by ::eaarlio_tld_opener_empty.
     */
    eaarlio_error (*close)(struct eaarlio_tld_opener *self);

    /**
     * Internal data pointer
     *
     * This is not used by calling code directly. It provides a means for the
     * functions in the structure to maintain internal state. If you do not
     * need it, you can set it to @c NULL.
     */
    void *opaque;
};

/**
 * Empty eaarlio_tld_opener value
 *
 * All pointers will be null.
 */
#define eaarlio_tld_opener_empty()                                             \
    (struct eaarlio_tld_opener)                                                \
    {                                                                          \
        NULL, NULL, NULL                                                       \
    }

#endif
