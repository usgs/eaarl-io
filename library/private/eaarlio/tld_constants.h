#ifndef EAARLIO_TLD_CONSTANTS_H
#define EAARLIO_TLD_CONSTANTS_H

/**
 * @file
 * @brief Constants for the TLD file format
 */

/**
 * Encoded byte size of a TLD record header
 */
#define EAARLIO_TLD_RECORD_HEADER_SIZE 4U

/**
 * Encoded byte size of a TLD raster header
 */
#define EAARLIO_TLD_RASTER_HEADER_SIZE 14U

/**
 * Encoded byte size of a TLD pulse header
 */
#define EAARLIO_TLD_PULSE_HEADER_SIZE 13U

/**
 * Encoded byte size of the length for a waveform data section
 */
#define EAARLIO_TLD_WF_DATA_LENGTH_SIZE 2U

/**
 * Encoded byte size of the length for a transmit waveform
 */
#define EAARLIO_TLD_TX_LENGTH_SIZE 1U

/**
 * Encoded byte size of the length for a return waveform
 */
#define EAARLIO_TLD_RX_LENGTH_SIZE 2U

#endif
