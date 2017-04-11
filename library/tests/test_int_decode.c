#include "eaarlio/int_decode.h"
#include "greatest.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "data_int.h"

TEST test_uint8_sanity()
{
    unsigned char buf[1];
    eaarlio_int_decode_uint8((unsigned char *)buf);
    PASS();
}

TEST test_uint8_data(char const *msg, char c1, uint8_t val)
{
    unsigned char buf[1];
    buf[0] = (unsigned char)c1;
    ASSERT_EQ_FMTm(
        msg, val, eaarlio_int_decode_uint8((unsigned char *)buf), "%u");
    PASS();
}

SUITE(suite_uint8)
{
    RUN_TEST(test_uint8_sanity);
    data_suite_int_uint8(test_uint8_data);
}

TEST test_uint16_sanity()
{
    unsigned char buf[2];
    eaarlio_int_decode_uint16((unsigned char *)buf);
    PASS();
}

TEST test_uint16_data(char const *msg, char c1, char c2, uint16_t val)
{
    unsigned char buf[2];
    buf[0] = (unsigned char)c1;
    buf[1] = (unsigned char)c2;
    ASSERT_EQ_FMTm(
        msg, val, eaarlio_int_decode_uint16((unsigned char *)buf), "%hu");
    PASS();
}

SUITE(suite_uint16)
{
    RUN_TEST(test_uint16_sanity);
    data_suite_int_uint16(test_uint16_data);
}

TEST test_uint24_sanity()
{
    unsigned char buf[3];
    eaarlio_int_decode_uint24((unsigned char *)buf);
    PASS();
}

TEST test_uint24_data(char const *msg, char c1, char c2, char c3, uint32_t val)
{
    unsigned char buf[3];
    buf[0] = (unsigned char)c1;
    buf[1] = (unsigned char)c2;
    buf[2] = (unsigned char)c3;
    ASSERT_EQ_FMTm(
        msg, val, eaarlio_int_decode_uint24((unsigned char *)buf), "%lu");

    PASS();
}

SUITE(suite_uint24)
{
    RUN_TEST(test_uint24_sanity);
    data_suite_int_uint24(test_uint24_data);
}

TEST test_uint32_sanity()
{
    unsigned char buf[4];
    eaarlio_int_decode_uint32((unsigned char *)buf);
    PASS();
}

TEST test_uint32_data(char const *msg,
    char c1,
    char c2,
    char c3,
    char c4,
    uint32_t val)
{
    unsigned char buf[4];
    buf[0] = (unsigned char)c1;
    buf[1] = (unsigned char)c2;
    buf[2] = (unsigned char)c3;
    buf[3] = (unsigned char)c4;
    ASSERT_EQ_FMTm(
        msg, val, eaarlio_int_decode_uint32((unsigned char *)buf), "%lu");
    PASS();
}

SUITE(suite_uint32)
{
    RUN_TEST(test_uint32_sanity);
    data_suite_int_uint32(test_uint32_data);
}

TEST test_int16_sanity()
{
    unsigned char buf[2];
    eaarlio_int_decode_int16((unsigned char *)buf);
    PASS();
}

TEST test_int16_data(char const *msg, char c1, char c2, int16_t val)
{
    unsigned char buf[2];
    buf[0] = (unsigned char)c1;
    buf[1] = (unsigned char)c2;
    ASSERT_EQ_FMTm(
        msg, val, eaarlio_int_decode_int16((unsigned char *)buf), "%hi");
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

    RUN_SUITE(suite_uint8);
    RUN_SUITE(suite_uint16);
    RUN_SUITE(suite_uint24);
    RUN_SUITE(suite_uint32);
    RUN_SUITE(suite_int16);

    GREATEST_MAIN_END();
}
