#ifndef EAARLIO_PULSE_H
#define EAARLIO_PULSE_H

/**
 * @file
 * @brief Structure and support for EAARL pulse data
 */

#include "eaarlio/error.h"
#include "eaarlio/memory.h"
#include <stdint.h>

/**
 * Maximum number of return waveforms that are possible for TLD data
 *
 * The file format does not support more than this.
 */
#define EAARLIO_MAX_RX_COUNT 4

/**
 * EAARL pulse record
 */
struct eaarlio_pulse {
    /**
     * Additional fractional time elapsed since raster timestamp
     *
     * Use ::eaarlio_units_pulse_time to derive floating point time.
     */
    uint32_t time_offset;

    /**
     * Number of return waveforms
     *
     * @warning This must not exceed ::EAARLIO_MAX_RX_COUNT.
     */
    uint8_t rx_count;

    /**
     * Angle of scanner, in hardware units
     *
     * Use ::eaarlio_units_pulse_scan_angle to derive angle in degrees.
     */
    int16_t scan_angle_counts;

    /**
     * Range measurement for laser
     *
     * Measurement is in nanoseconds. Measures travel time from mirror to start
     * of raster and back.
     */
    uint16_t range;

    /**
     * Transmit waveform hardware threshold failure
     *
     * 1 indicates that the transmit waveform failed the hardware defined
     * treshold
     */
    uint8_t thresh_tx;

    /**
     * Return waveform hardware threshold failure
     *
     * 1 indicates that the return waveforms failed the hardware defined
     * treshold
     */
    uint8_t thresh_rx;

    /** Transmit range bias in nanoseconds */
    uint8_t bias_tx;

    /** Channel range bias in nanoseconds */
    uint8_t bias_rx[EAARLIO_MAX_RX_COUNT];

    /** Length of eaarlio_pulse::tx */
    uint8_t tx_len;

    /** Lengths of eaarlio_pulse::rx */
    uint16_t rx_len[EAARLIO_MAX_RX_COUNT];

    /** Transmit waveform */
    unsigned char *tx;

    /** Return waveforms */
    unsigned char *rx[EAARLIO_MAX_RX_COUNT];
};

/**
 * Empty ::eaarlio_pulse value
 *
 * All numeric fields will contain zero values. All pointers will be null.
 */
#define eaarlio_pulse_empty()                                                  \
    (struct eaarlio_pulse)                                                     \
    {                                                                          \
        .time_offset = 0, .rx_count = 0, .scan_angle_counts = 0, .range = 0,   \
        .thresh_tx = 0, .thresh_rx = 0, .bias_tx = 0,                          \
        .bias_rx = { 0, 0, 0, 0 }, .tx_len = 0, .tx = NULL,                    \
        .rx_len = { 0, 0, 0, 0 }, .rx = {                                      \
            NULL,                                                              \
            NULL,                                                              \
            NULL,                                                              \
            NULL                                                               \
        }                                                                      \
    }

/**
 * Release memory held within an ::eaarlio_pulse
 *
 * @param[in,out] pulse Pulse with memory to release
 * @param[in] memory Memory handler, or NULL for stdlib
 *
 * @returns_eaarlio_error
 *
 * @post On success, all pointer fields (pulse->tx, pulse->rx[0], pulse->rx[1],
 *      pulse->rx[2], pulse->rx[3]) will be null.
 * @post On success, any memory that was allocated for the pointer fields has
 *      been released.
 *
 * @remark The pointer to @p pulse is not released.
 * @remark Pointer fields may be null. If so, they are left alone.
 */
eaarlio_error eaarlio_pulse_free(struct eaarlio_pulse *pulse,
    struct eaarlio_memory *memory);

#endif
