#ifndef EAARLIO_TLD_DECODE_H
#define EAARLIO_TLD_DECODE_H

/**
 * @file
 * @brief Low-level decoding of TLD data
 *
 * This header provides functionality to decode TLD data from buffers (arrays
 * of unsigned char). The functionality is considered low level because:
 *      - The caller is responsible for allocating any memory needed for the
 *        decoded data.
 *      - The caller is responsible for retrieving the raw data from its
 *        underlying source (typically a file).
 */

#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include <stdint.h>

/**
 * Decode a record header.
 *
 * On success, @p record_header is populated. On failure, @p record_header is
 * not modified.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] record_header Pointer to single header value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_decode_record_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_tld_header *record_header);

/**
 * Decode a raster header.
 *
 * On success, @p raster is populated. On failure, @p raster is not modified.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] raster Pointer to single raster whose header values should be
 *      populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_decode_raster_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster *raster);

/**
 * Decode a pulse header.
 *
 * On success, @p pulse is populated. On failure, @p pulse is not modified.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] pulse Pointer to single pulse whose header values should be
 *      populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_decode_pulse_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_pulse *pulse);

/**
 * Decode the length of the waveform data segment following a pulse header.
 *
 * On success, @p length is populated. On failure, @p length is not modified.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] length Pointer to single length value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_decode_wf_data_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t *length);

/**
 * Decode the length of a transmit waveform.
 *
 * On success, @p length is populated. On failure, @p length is not modified.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] length Pointer to single length value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_decode_tx_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint8_t *length);

/**
 * Decode the length of a return waveform.
 *
 * On success, @p length is populated. On failure, @p length is not modified.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] length Pointer to single length value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_decode_rx_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t *length);

/**
 * Decode a waveform.
 *
 * On success, @p length is populated. On failure, @p length is not modified.
 *
 * If neither pointer is null and @p waveform_len is 0, then this is a no-op
 * and returns success.
 *
 * @param[in] buffer Raw data to decode
 * @param[in] buffer_len Length of @p buffer
 * @param[out] waveform Pointer to waveform array to be populated
 * @param[in] waveform_len Length of @p waveform
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_decode_waveform(unsigned char const *buffer,
    uint32_t buffer_len,
    unsigned char *waveform,
    uint16_t waveform_len);

#endif
