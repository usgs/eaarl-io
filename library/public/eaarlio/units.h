#ifndef EAARLIO_UNITS_H
#define EAARLIO_UNITS_H

/**
 * @file
 * @brief Unit conversions for EAARL data
 *
 * This header provides constants and functions for use in converting
 * hardware-specific units to standard units for time and angles.
 */

#include "eaarlio/edb.h"
#include "eaarlio/raster.h"
#include <stdint.h>

/******************************************************************************/

/**
 * Conversion factor for scan angle counts to degrees
 *
 * The raw value for scan angle counts (::eaarlio_pulse::scan_angle_counts)
 * needs to be multiplied by this to convert it into degrees.
 */
#define EAARLIO_UNITS_SCAN_ANGLE_COUNTS_DEGREES 0.045

/**
 * Conversion factor for fractional time to seconds
 *
 * The raw values for fractional times (::eaarlio_raster::time_fraction and
 * ::eaarlio_pulse::time_offset) need to be multiplied by this to convert
 * them to seconds.
 */
#define EAARLIO_UNITS_TIME_FRACTION_SECONDS 1.6e-6

/******************************************************************************/

/**
 * Returns the timestamp for an EDB record
 *
 * Uses the ::eaarlio_edb_record::time_seconds and
 * ::eaarlio_edb_record::time_fraction fields to derive a timestamp in seconds
 * of the epoch.
 *
 * @param[in] record An EDB record
 *
 * @returns The seconds-of-the-epoch timestamp for the record, or 0 if @p
 *      record is @c NULL
 */
double eaarlio_units_edb_time(struct eaarlio_edb_record *record);

/**
 * Returns the timestamp for an EAARL raster
 *
 * Uses the ::eaarlio_raster::time_seconds and ::eaarlio_raster::time_fraction
 * fields to derive a timestamp in seconds of the epoch.
 *
 * @param[in] raster An EAARL raster
 *
 * @returns The seconds-of-the-epoch timestamp for the raster, or 0 if @p
 *      raster is @c NULL
 */
double eaarlio_units_raster_time(struct eaarlio_raster *raster);

/**
 * Returns the timestamp for a pulse in an EAARL raster
 *
 * Uses the ::eaarlio_raster::time_seconds and ::eaarlio_raster::time_fraction
 * fields and the ::eaarlio_pulse::time_offset field from the specified pulse
 * to derive a timestamp in seconds of the epoch.
 *
 * @param[in] raster An EAARL raster
 * @param[in] pulse_number The pulse number, which is a 1-based index into
 *      ::eaarlio_raster::pulse.
 *
 * @returns The seconds-of-the-epoch timestamp for the pulse, or 0 if @p
 *      raster is @c NULL or if the pulse cannot be retrieved
 */
double eaarlio_units_pulse_time(struct eaarlio_raster *raster,
    uint16_t pulse_number);

/**
 * Returns the scan angle in degrees for a pulse
 *
 * Uses the ::eaarlio_pulse::scan_angle_counts field to derive the scan
 * angle in degrees.
 *
 * @param[in] pulse An EAARL pulse
 *
 * @returns The scan angle in degrees, or 0 if @p pulse is @c NULL
 */
double eaarlio_units_pulse_scan_angle(struct eaarlio_pulse *pulse);

#endif
