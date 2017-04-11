#ifndef EAARLIO_TLD_ENCODE_H
#define EAARLIO_TLD_ENCODE_H

/**
 * @file
 * @brief Low-level encoding of TLD data
 *
 * This header provides functionality to encode TLD data to buffers (arrays
 * of unsigned char). The functionality is considered low level because:
 *      - The caller is responsible for allocating any memory needed for
 *        buffers.
 *      - The caller is responsible for storing the encoded data to whatever
 *        underlying destination is desired (typically a file), if applicable.
 */

#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include <stdint.h>

/**
 * Encode a record header.
 *
 * On success, @p buffer is populated. On failure, @p buffer is not modified.
 *
 * Some fields are constrained to a limited range of values for encoding.
 *
 * | Field                           | Min | Max      |
 * | ------------------------------- | --- | -------- |
 * | @p record_header->record_length | 0   | 16777215 |
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Length of @p buffer
 * @param[in] record_header Pointer to single header value to be encoded
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_encode_record_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_tld_header const *record_header);

/**
 * Encode a raster header.
 *
 * On success, @p buffer is populated. On failure, @p buffer is not modified.
 *
 * Some fields are constrained to a limited range of values for encoding.
 *
 * | Field                  | Min | Max   |
 * | ---------------------- | --- | ----- |
 * | @p raster->pulse_count | 0   | 32767 |
 * | @p raster->digitizer   | 0   | 1     |
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Length of @p buffer
 * @param[in] raster Pointer to single raster whose header value should be
 *      encoded
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_encode_raster_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster const *raster);

/**
 * Encode a pulse header.
 *
 * On success, @p buffer is populated. On failure, @p buffer is not modified.
 *
 * Some fields are constrained to a limited range of values for encoding.
 *
 * | Field               | Min | Max   |
 * | ------------------- | --- | ----- |
 * | @p pulse->range     | 0   | 16383 |
 * | @p pulse->thresh_tx | 0   | 1     |
 * | @p pulse->thresh_rx | 0   | 1     |
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Length of @p buffer
 * @param[in] pulse Pointer to single pulse whose header value should be
 *      encoded
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_encode_pulse_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_pulse const *pulse);

/**
 * Encode the length of the waveform data segment following a pulse header.
 *
 * On success, @p buffer is populated. On failure, @p buffer is not modified.
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Length of @p buffer
 * @param[in] length Length value to be encoded
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_encode_wf_data_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint16_t length);

/**
 * Encode the length of a transmit waveform.
 *
 * On success, @p buffer is populated. On failure, @p buffer is not modified.
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Length of @p buffer
 * @param[in] length Length value to be encoded
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_encode_tx_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint8_t length);

/**
 * Encode the length of a return waveform.
 *
 * On success, @p buffer is populated. On failure, @p buffer is not modified.
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Length of @p buffer
 * @param[in] length Length value to be encoded
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_encode_rx_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint16_t length);

/**
 * Encode a waveform.
 *
 * On success, @p buffer is populated. On failure, @p buffer is not modified.
 *
 * If neither pointer is null and @p waveform_len is 0, then this is a no-op
 * and returns success.
 *
 * @param[out] buffer Destination for encoded data
 * @param[in] buffer_len Length of @p buffer
 * @param[in] waveform Pointer to waveform data to be encoded
 * @param[in] waveform_len Length of @p waveform
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_encode_waveform(unsigned char *buffer,
    uint32_t buffer_len,
    unsigned char const *waveform,
    uint16_t waveform_len);

#endif
