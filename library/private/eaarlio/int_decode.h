#ifndef INT_DECODE_H
#define INT_DECODE_H

/**
 * @file
 * @brief Little endian integer decoding
 *
 * This header provides functions for decoding little endian values from byte
 * arrays.
 *
 * These functions do not do any error checking. Do not provide null pointers.
 * Ensure your buffers are appropriately sized.
 */

#include <stdint.h>

/**
 * Decode 8-bit unsigned integer
 *
 * @param[in] buf Bytes to decode
 * @returns 8-bit unsigned integer
 */
uint8_t eaarlio_int_decode_uint8(unsigned char const *const buf);

/**
 * Decode 16-bit unsigned integer
 *
 * @param[in] buf Bytes to decode
 * @returns 16-bit unsigned integer
 */
uint16_t eaarlio_int_decode_uint16(unsigned char const *const buf);

/**
 * Decode 24-bit unsigned integer
 *
 * @param[in] buf Bytes to decode
 * @returns 32-bit unsigned integer containing a 24-bit unsigned integer
 */
uint32_t eaarlio_int_decode_uint24(unsigned char const *const buf);

/**
 * Decode 32-bit unsigned integer
 *
 * @param[in] buf Bytes to decode
 * @returns 32-bit unsigned integer
 */
uint32_t eaarlio_int_decode_uint32(unsigned char const *const buf);

/**
 * Decode 16-bit signed integer
 *
 * @param[in] buf Bytes to decode
 * @returns 16-bit signed integer
 */
int16_t eaarlio_int_decode_int16(unsigned char const *const buf);

#endif
