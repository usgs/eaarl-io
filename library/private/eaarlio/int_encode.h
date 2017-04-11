#ifndef INT_ENCODE_H
#define INT_ENCODE_H

/**
 * @file
 * @brief Little endian integer encoding
 *
 * This header provides functions for encoding little endian values to byte
 * arrays.
 *
 * These functions do not do any error checking. Do not provide null pointers.
 * Ensure your buffers are appropriately sized. Ensure your values are within
 * the appropriate ranges.
 */

#include <stdint.h>

/**
 * Encode 8-bit unsigned integer
 *
 * @param[out] buf Destination for encoded bytes
 * @param[in] val Integer to encode
 */
void eaarlio_int_encode_uint8(unsigned char *buf, uint8_t val);

/**
 * Encode 16-bit unsigned integer
 *
 * @param[out] buf Destination for encoded bytes
 * @param[in] val Integer to encode
 */
void eaarlio_int_encode_uint16(unsigned char *buf, uint16_t val);

/**
 * Encode 24-bit unsigned integer
 *
 * @param[out] buf Destination for encoded bytes
 * @param[in] val Integer to encode
 */
void eaarlio_int_encode_uint24(unsigned char *buf, uint32_t val);

/**
 * Encode 32-bit unsigned integer
 *
 * @param[out] buf Destination for encoded bytes
 * @param[in] val Integer to encode
 */
void eaarlio_int_encode_uint32(unsigned char *buf, uint32_t val);

/**
 * Encode 16-bit signed integer
 *
 * @param[out] buf Destination for encoded bytes
 * @param[in] val Integer to encode
 */
void eaarlio_int_encode_int16(unsigned char *buf, int16_t val);

#endif
