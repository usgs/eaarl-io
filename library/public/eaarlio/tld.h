#ifndef EAARLIO_TLD_H
#define EAARLIO_TLD_H

/**
 * @file
 * @brief Read/write support for the TLD file format
 */

#include "eaarlio/error.h"
#include "eaarlio/memory.h"
#include "eaarlio/raster.h"
#include "eaarlio/stream.h"
#include <stdint.h>

/******************************************************************************/

/**
 * Type value for an EAARL-A/B raster record
 *
 * Only records with ::eaarlio_tld_header::record_type ==
 * ::EAARLIO_TLD_TYPE_RASTER are understood to be rasters by this library.
 */
#define EAARLIO_TLD_TYPE_RASTER 5

/******************************************************************************/

/**
 * TLD record header
 *
 * Each record in a TLD file starts with one of these.
 */
struct eaarlio_tld_header {
    /** Byte length of this record (including the header) */
    uint32_t record_length;
    /**
     * Record type
     *
     * This is always ::EAARLIO_TLD_TYPE_RASTER (5) for EAARL-A/EAARL-B.
     */
    uint8_t record_type;
};

/**
 * Empty eaarlio_tld_header value
 *
 * All fields will contain zero values.
 */
#define eaarlio_tld_header_empty()                                             \
    (struct eaarlio_tld_header)                                                \
    {                                                                          \
        0, 0                                                                   \
    }

/**
 * Read a TLD record from a stream
 *
 * @param[in] stream Stream with data to read
 * @param[out] record_header Pointer to record header to be populated
 * @param[out] raster Pointer to raster to be populated
 * @param[in] memory Memory handler, or NULL for stdlib
 * @param[in] include_pulses Should pulse data be read? 1 = yes, 0 = no
 * @param[in] include_waveforms Should waveform data be read? 1 = yes, 0 = no
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for reading.
 *
 * @post On success, the stream is advanced to the end of the record.
 * @post On success, @p record_header is populated.
 * @post On success, if @p record_header->record_type is
 *      ::EAARLIO_TLD_TYPE_RASTER, then @p raster->header is populated.
 * @post On success, if @p record_header->record_type is
 *      ::EAARLIO_TLD_TYPE_RASTER and include_pulses = 1, then @p raster->pulse
 *      is populated; each pulse will have its header populated.
 * @post On success, if @p record_header->record_type is
 *      ::EAARLIO_TLD_TYPE_RASTER and include_pulses = 1 and include_waveforms =
 *      1, then the waveforms for each pulse in @p raster->pulse are populated.
 * @post On failure, anything might be partially populated.
 * @post Any non-null pointers in @p raster are newly-allocated memory.
 */
eaarlio_error eaarlio_tld_read_record(struct eaarlio_stream *stream,
    struct eaarlio_tld_header *record_header,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_pulses,
    int include_waveforms);

/**
 * Read a raster from a TLD stream
 *
 * This function is convenience wrapper around ::eaarlio_tld_read_record and
 * works nearly identically to it. The differences are:
 * - This function does not have a record_header parameter
 * - When the record type is not ::EAARLIO_TLD_TYPE_RASTER, this function
 *   returns ::EAARLIO_TLD_TYPE_UNKNOWN instead of ::EAARLIO_SUCCESS.
 *
 * Please refer to ::eaarlio_tld_read_record for further documentation.
 */
eaarlio_error eaarlio_tld_read_raster(struct eaarlio_stream *stream,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_pulses,
    int include_waveforms);

/**
 * Write a raster to a TLD stream
 *
 * @param[in] stream Stream to be written to
 * @param[in] raster Raster to write
 * @param[in] memory Memory handler, or NULL for stdlib
 *
 * @returns_eaarlio_error
 *
 * @pre @p stream must be open for writing
 *
 * @post On success, the encoded data was written to @p stream.
 * @post On failure, the encoded data may have been partially written to @p
 *      stream.
 *
 * @remark The raster will be written as a TLD record of type
 *      ::EAARLIO_TLD_TYPE_RASTER.
 * @remark This function performs some memory allocation internally. However,
 *      all allocated memory is released before the function returns.
 */
eaarlio_error eaarlio_tld_write_raster(struct eaarlio_stream *stream,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory);

#endif
