#ifndef EAARLIO_TLD_PACK_H
#define EAARLIO_TLD_PACK_H

/**
 * @file
 * @brief Mid-level packing of TLD data
 *
 * This header provides functionality to pack TLD data to buffers (arrays
 * of unsigned char). The functionality is considered mid level because:
 *      - The functions perform memory allocation.
 *      - The caller is responsible for storing the encoded data to whatever
 *        underlying destination is desired (typically a file), if applicable.
 *
 * The functions in this header are *not* sufficient for fully encoding a TLD
 * record. They are meant to supplement the functions from @ref tld_encode.h.
 */

#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include <stdint.h>

/**
 * Pack a raster into a buffer
 *
 * This function encodes a full raster to a buffer. This encapsulates the
 * following lower-level functions:
 *      - ::eaarlio_tld_size_raster
 *      - ::eaarlio_tld_encode_raster_header
 *      - ::eaarlio_tld_encode_pulse_header
 *      - ::eaarlio_tld_size_pulse_waveforms
 *      - ::eaarlio_tld_encode_wf_data_length
 *      - ::eaarlio_tld_encode_tx_length
 *      - ::eaarlio_tld_encode_rx_length
 *      - ::eaarlio_tld_encode_waveform
 *
 * On success, @p buffer points to newly allocated memory that is populated
 * with the encoded raster and @p buffer_len points to the size of the newly
 * allocated memory.
 *
 * On failure, @p buffer and @p buffer_len may be partially populated. Any
 * non-null pointers are newly allocated memory.
 *
 * @param[out] buffer Destination for encoded data
 * @param[out] buffer_len Length of @p buffer
 * @param[in] raster Pointer to single raster to be encoded
 * @param[in] memory Memory handler, or @c NULL for stdlib
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_pack_raster(unsigned char **buffer,
    uint32_t *buffer_len,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory);

#endif
