#ifndef DATA_INTEGER_H
#define DATA_INTEGER_H

#include <stdint.h>

#include "greatest_extend.h"

/**
 * @file
 * @brief Data suites for int_encode and int_decode functions
 *
 * The data suites each accept a test function that accepts similar parameters.
 *
 * @param msg A label for the test
 * @param c1 First byte of encoded value
 * @param c2 Second byte of encoded value
 * @param c3 Third byte of encoded value
 * @param c4 Fourth byte of encoded value
 * @param val Decoded value
 */

void data_suite_int_uint8(
    TEST_TYPE (*test)(char const *msg, char c1, uint8_t val));

void data_suite_int_uint16(
    TEST_TYPE (*test)(char const *msg, char c1, char c2, uint16_t val));

void data_suite_int_int16(
    TEST_TYPE (*test)(char const *msg, char c1, char c2, int16_t val));

void data_suite_int_uint24(TEST_TYPE (
    *test)(char const *msg, char c1, char c2, char c3, uint32_t val));

void data_suite_int_uint32(TEST_TYPE (
    *test)(char const *msg, char c1, char c2, char c3, char c4, uint32_t val));

#endif
