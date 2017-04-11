#ifndef EAARLIO_RASTER_H
#define EAARLIO_RASTER_H

/**
 * @file
 * @brief Constants, structures, and utility functions for EAARL raster data
 *
 * This header provides the structures used for working with EAARL raster data.
 * These structures are used throughout the rest of the TLD related parts of
 * the API.
 *
 * Some utility functions are also included for releasing memory allocated for
 * ::eaarlio_raster and ::eaarlio_pulse records.
 */

#include "eaarlio/memory.h"
#include "eaarlio/pulse.h"
#include <stdint.h>

/**
 * EAARL raster record
 */
struct eaarlio_raster {
    /**
     * Integer seconds component of timestamp
     *
     * Use ::eaarlio_units_raster_time to derive floating point time
     */
    uint32_t time_seconds;

    /**
     * Fractional seconds component of timestamp
     *
     * Use ::eaarlio_units_raster_time to derive floating point time
     */
    uint32_t time_fraction;

    /**
     * Sequence number, ignore
     *
     * This value cycles and repeats and is thus not suitable for uniquely
     * identifying rasters. The value is not used for anything and should be
     * ignored.
     */
    uint32_t sequence_number;

    /** Number of pulses in this raster */
    uint16_t pulse_count;

    /** Digitizer used: 0 or 1 */
    uint8_t digitizer;

    /**
     * Pulse records
     *
     * The size is specified by ::eaarlio_raster::pulse_count.
     */
    struct eaarlio_pulse *pulse;
};

/**
 * Empty ::eaarlio_raster value
 *
 * All numeric fields will contain zero values. The pointer will be null.
 */
#define eaarlio_raster_empty()                                                 \
    (struct eaarlio_raster)                                                    \
    {                                                                          \
        0, 0, 0, 0, 0, NULL                                                    \
    }

/**
 * Free memory allocated to fields in ::eaarlio_raster
 *
 * @param[in,out] raster Raster with memory to release
 * @param[in] memory Memory handler, or NULL for stdlib
 *
 * @returns_eaarlio_error
 *
 * @post On success, @p raster->pulse will be null.
 * @post On success, any memory that was allocated for @p raster->pulse has
 *      been released.
 *
 * @remark The pointer to @p raster is not released.
 * @remark @p raster->pulse may be null. If so, it is left alone.
 */
eaarlio_error eaarlio_raster_free(struct eaarlio_raster *raster,
    struct eaarlio_memory *memory);

#endif
