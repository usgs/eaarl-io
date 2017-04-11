#ifndef EAARLIO_TLD_UNPACK_H
#define EAARLIO_TLD_UNPACK_H

/**
 * @file
 * @brief Mid-level unpacking of TLD data
 *
 * This header provides functionality to unpack TLD data from buffers (arrays
 * of unsigned char). The functionality is considered mid level because:
 *      - The functions perform memory allocation.
 *      - The caller is responsible for retrieving the raw data from its
 *        underlying source (typically a file).
 *
 * The functions in this header are *not* sufficient for fully decoding a TLD
 * record. They are meant to supplement the functions from @ref tld_decode.h.
 */

#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include <stdint.h>

/**
 * Unpack the waveform data for a pulse
 *
 * This function decodes the waveform data block of a pulse record, performing
 * memory allocation as needed. This encapsulates the following lower-level
 * functions:
 *      - ::eaarlio_tld_decode_tx_length
 *      - ::eaarlio_tld_decode_rx_length
 *      - ::eaarlio_tld_decode_waveform
 *
 * The following fields may have memory allocated:
 *      - @p pulse->tx
 *      - @p pulse->rx[0]
 *      - @p pulse->rx[1]
 *      - @p pulse->rx[2]
 *      - @p pulse->rx[3]
 *
 * On success, @p pulse->tx_len, @p pulse->tx, @p pulse->rx_len, and
 * @p pulse->rx are fully populated.
 *
 * On failure, @p pulse->tx_len, @p pulse->tx, @p pulse->rx_len, and
 * @p pulse->rx may be partially populated. Any pointers not populated will be
 * set to null. Any non-null pointers are newly allocated memory.
 *
 * On failure with ::EAARLIO_BUFFER_SHORT, one or more waveforms may have
 * been truncated. This is not necessarily a true failure: EAARL TLD data
 * typically truncates the final waveform of the final pulse of each raster.
 * When a truncated waveform is encountered, the corresponding waveform length
 * field (@p tx_len or @p rx_len) is reduced to match and the data available is
 * used to populate the relevant waveform field (@p tx or @p rx).
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] pulse Pointer to a single pulse value to be populated
 * @param[in] memory Memory handler, or @c NULL for stdlib
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_unpack_waveforms(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_pulse *pulse,
    struct eaarlio_memory *memory);

/**
 * Unpack the pulses data for a raster
 *
 * This function decodes the header data for each pulse in a raster, performing
 * memory allocation as needed. This encapsulates the following lower-level
 * functions:
 *      - ::eaarlio_tld_decode_pulse_header
 *      - ::eaarlio_tld_decode_wf_data_length
 *
 * The following fields may have memory allocated:
 *      - @p raster->pulse
 *      - fields noted for ::eaarlio_tld_unpack_waveforms
 *
 * If @p include_waveforms is non-zero, then ::eaarlio_tld_unpack_waveforms
 * is also called on each pulse to decode its waveform data. If
 * @p include_waveforms is zero, then the waveform-related fields for each
 * pulse (@p pulse->tx, @p pulse->tx_len, @p pulse->rx, and @p pulse->rx_len)
 * are not populated; the pointers are set to NULL.
 *
 * On success, @p raster->pulse is fully populated.
 *
 * On failure, @p raster->pulse may be partially populated. Any pointers not
 * populated will be set to null. Any non-null pointers are newly allocated
 * memory.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] raster Pointer to a single raster value to be populated
 * @param[in] memory Memory handler, or @c NULL for stdlib
 * @param[in] include_waveforms Should waveform data be unpacked? 1 = yes, 0 =
 *      no
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_unpack_pulses(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_waveforms);

/**
 * Unpack the data for a raster
 *
 * This function decodes the header data for a raster. This encapsulates the
 * following lower-level function:
 *      - ::eaarlio_tld_decode_raster_header
 *
 * The following fields may have memory allocated:
 *      - fields noted for ::eaarlio_tld_unpack_pulses
 *      - fields noted for ::eaarlio_tld_unpack_waveforms
 *
 * If @p include_pulses is non-zero, then ::eaarlio_tld_unpack_pulses is
 * also called to decode the pulse data for the raster. If @p include_pulses is
 * zero, then @p raster->pulse is set to NULL and no memory allocation occurs.
 *
 * @p include pulses is passed through to ::eaarlio_tld_unpack_pulses and
 * determines whether ::eaarlio_tld_unpack_waveforms is called for each
 * pulse.
 *
 * On success, @p raster is fully populated.
 *
 * On failure, @p raster may be partially populated. Any pointers not populated
 * will be set to null. Any non-null pointers are newly allocated memory.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] raster Pointer to a single raster value to be populated
 * @param[in] memory Memory handler, or @c NULL for stdlib
 * @param[in] include_pulses Should pulse data be unpacked? 1 = yes, 0 = no
 * @param[in] include_waveforms Should waveform data be unpacked? 1 = yes, 0 =
 *      no
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_unpack_raster(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_pulses,
    int include_waveforms);

#endif
