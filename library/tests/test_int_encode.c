#include "eaarlio/int_encode.h"
#include "greatest.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "data_int.h"

unsigned char *expected = NULL;
unsigned char *actual = NULL;

TEST test_uint8_sanity()
{
    eaarlio_int_encode_uint8(actual, 0);
    PASS();
}

TEST test_uint8_data(char const *msg, char c1, uint8_t val)
{
    expected[0] = (unsigned char)c1;
    eaarlio_int_encode_uint8(actual, val);
    ASSERT_MEM_EQm(msg, expected, actual, 1);
    PASS();
}

SUITE(suite_uint8)
{
    RUN_TEST(test_uint8_sanity);
    data_suite_int_uint8(test_uint8_data);
}

TEST test_uint16_sanity()
{
    eaarlio_int_encode_uint16(expected, 0);
    PASS();
}

TEST test_uint16_data(char const *msg, char c1, char c2, uint16_t val)
{
    expected[0] = (unsigned char)c1;
    expected[1] = (unsigned char)c2;
    eaarlio_int_encode_uint16(actual, val);
    ASSERT_MEM_EQm(msg, expected, actual, 2);
    PASS();
}

SUITE(suite_uint16)
{
    RUN_TEST(test_uint16_sanity);
    data_suite_int_uint16(test_uint16_data);
}

TEST test_uint24_sanity()
{
    eaarlio_int_encode_uint24(actual, 0);
    PASS();
}

TEST test_uint24_data(char const *msg, char c1, char c2, char c3, uint32_t val)
{
    expected[0] = (unsigned char)c1;
    expected[1] = (unsigned char)c2;
    expected[2] = (unsigned char)c3;
    eaarlio_int_encode_uint24(actual, val);
    ASSERT_MEM_EQm(msg, expected, actual, 3);
    PASS();
}

SUITE(suite_uint24)
{
    RUN_TEST(test_uint24_sanity);
    data_suite_int_uint24(test_uint24_data);
}

TEST test_uint32_sanity()
{
    eaarlio_int_encode_uint32(actual, 0);
    PASS();
}

TEST test_uint32_data(char const *msg,
    char c1,
    char c2,
    char c3,
    char c4,
    uint32_t val)
{
    expected[0] = (unsigned char)c1;
    expected[1] = (unsigned char)c2;
    expected[2] = (unsigned char)c3;
    expected[3] = (unsigned char)c4;
    eaarlio_int_encode_uint32(actual, val);
    ASSERT_MEM_EQm(msg, expected, actual, 4);
    PASS();
}

SUITE(suite_uint32)
{
    RUN_TEST(test_uint32_sanity);
    data_suite_int_uint32(test_uint32_data);
}

TEST test_int16_sanity()
{
    eaarlio_int_encode_int16(actual, 0);
    PASS();
}

TEST test_int16_data(char const *msg, char c1, char c2, int16_t val)
{
    expected[0] = (unsigned char)c1;
    expected[1] = (unsigned char)c2;
    eaarlio_int_encode_int16(actual, val);
    ASSERT_MEM_EQm(msg, expected, actual, 2);

    PASS();
}

SUITE(suite_int16)
{
    RUN_TEST(test_int16_sanity);
    data_suite_int_int16(test_int16_data);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();

    expected = malloc(4);
    assert(expected);
    actual = malloc(4);
    assert(actual);

    RUN_SUITE(suite_uint8);
    RUN_SUITE(suite_uint16);
    RUN_SUITE(suite_uint24);
    RUN_SUITE(suite_uint32);
    RUN_SUITE(suite_int16);

    free(expected);
    free(actual);

    GREATEST_MAIN_END();
}
