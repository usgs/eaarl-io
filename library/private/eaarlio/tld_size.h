#ifndef EAARLIO_TLD_SIZE_H
#define EAARLIO_TLD_SIZE_H

/**
 * @file
 * @brief Calculates encoded size of TLD data
 *
 * This header provides functionality to calculate the size required to encode
 * TLD data.
 */

#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld.h"
#include <stdint.h>

/**
 * Calculate encoded size of a pulse's waveform data.
 *
 * This provides the size in bytes that the pulse's waveform data will occupy
 * when encoded for the TLD format. It is intended for use in calculating the
 * buffer size needed when encoding.
 *
 * On success, @p size is populated. On failure, @p size is set to 0 if
 * possible.
 *
 * @param[in] pulse Pointer to a single pulse
 * @param[out] size Pointer to a single size value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_size_pulse_waveforms(
    struct eaarlio_pulse const *pulse,
    uint16_t *size);

/**
 * Calculate encoded size of a pulse's data.
 *
 * This provides the size in bytes that the pulse's data (header plus
 * waveforms) will occupy when encoded for the TLD format. It is intended for
 * use in calculating the buffer size needed when encoding.
 *
 * On success, @p size is populated. On failure, @p size is set to 0 if
 * possible.
 *
 * @param[in] pulse Pointer to a single pulse
 * @param[out] size Pointer to a single size value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_size_pulse(struct eaarlio_pulse const *pulse,
    uint32_t *size);

/**
 * Calculate encoded size of a raster's pulse data.
 *
 * This provides the size in bytes that the raster's pulse data will occupy
 * when encoded for the TLD format. It is intended for use in calculating the
 * buffer size needed when encoding.
 *
 * Even though size is a pointer to a 32-bit unsigned integer, the encoded size
 * for a raster is bounded to 24 bits. Thus this function bounds the
 * permissible range of the size to 24 bits as well.
 *
 * On success, @p size is populated. On failure, @p size is set to 0 if
 * possible.
 *
 * @param[in] raster Pointer to a single raster
 * @param[out] size Pointer to a single size value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_size_raster_pulses(
    struct eaarlio_raster const *raster,
    uint32_t *size);

/**
 * Calculate encoded size of a raster's data.
 *
 * This provides the size in bytes that the raster's data (header plus pulses)
 * will occupy when encoded for the TLD format. It is intended for use in
 * calculating the buffer size needed when encoding.
 *
 * Even though size is a pointer to a 32-bit unsigned integer, the encoded size
 * for a raster is bounded to 24 bits. Thus this function bounds the
 * permissible range of the size to 24 bits as well.
 *
 * On success, @p size is populated. On failure, @p size is set to 0 if
 * possible.
 *
 * @param[in] raster Pointer to a single raster
 * @param[out] size Pointer to a single size value to be populated
 *
 * @returns_eaarlio_error
 */
eaarlio_error eaarlio_tld_size_raster(struct eaarlio_raster const *raster,
    uint32_t *size);

#endif
