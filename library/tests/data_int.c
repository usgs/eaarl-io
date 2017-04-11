#include "data_int.h"

void data_suite_int_uint8(
    TEST_TYPE (*test)(char const *msg, char c1, uint8_t val))
{
    RUN_MSG_INIT();
    RUN_MSG_FORMAT("0x%02hhx = %u");
    RUN_MSG(test, '\x2A', 42U);
    RUN_MSG(test, '\x7F', 127U);
    RUN_MSG(test, '\x80', 128U);
    RUN_MSG(test, '\xAC', 172U);
    RUN_MSG(test, '\xD6', 214U);
    RUN_MSG(test, '\xFF', 255U);
    RUN_MSG(test, '\x00', 0U);
}

void data_suite_int_uint16(
    TEST_TYPE (*test)(char const *msg, char c1, char c2, uint16_t val))
{
    RUN_MSG_INIT();
    RUN_MSG_FORMAT("0x%02hhx 0x%02hhx = %hu");
    RUN_MSG(test, '\x00', '\x01', 256U);
    RUN_MSG(test, '\x00', '\x2A', 10752U);
    RUN_MSG(test, '\x00', '\x80', 32768U);
    RUN_MSG(test, '\x11', '\x0F', 3857U);
    RUN_MSG(test, '\x17', '\xFB', 64279U);
    RUN_MSG(test, '\x2A', '\x00', 42U);
    RUN_MSG(test, '\xFF', '\x7F', 32767U);
    RUN_MSG(test, '\xFF', '\xFF', 65535U);
    RUN_MSG(test, '\x00', '\x00', 0U);
}

void data_suite_int_int16(
    TEST_TYPE (*test)(char const *msg, char c1, char c2, int16_t val))
{
    RUN_MSG_INIT();
    RUN_MSG_FORMAT("0x%02hhx 0x%02hhx = %hi");
    RUN_MSG(test, '\x00', '\x01', 256);
    RUN_MSG(test, '\x00', '\x2A', 10752);
    RUN_MSG(test, '\x00', '\x80', -32768);
    RUN_MSG(test, '\x11', '\x0F', 3857);
    RUN_MSG(test, '\x17', '\xFB', -1257);
    RUN_MSG(test, '\x2A', '\x00', 42);
    RUN_MSG(test, '\xFF', '\x7F', 32767);
    RUN_MSG(test, '\xFF', '\xFF', -1);
    RUN_MSG(test, '\x00', '\x00', 0);
}

void data_suite_int_uint24(
    TEST_TYPE (*test)(char const *msg, char c1, char c2, char c3, uint32_t val))
{
    RUN_MSG_INIT();
    RUN_MSG_FORMAT("0x%02hhx 0x%02hhx 0x%02hhx = %lu");
    RUN_MSG(test, '\x00', '\x01', '\x00', 256U);
    RUN_MSG(test, '\x00', '\x2A', '\x00', 10752U);
    RUN_MSG(test, '\x00', '\x80', '\xFF', 16744448U);
    RUN_MSG(test, '\x11', '\x0F', '\x00', 3857U);
    RUN_MSG(test, '\x17', '\xFB', '\xFF', 16775959U);
    RUN_MSG(test, '\x2A', '\x00', '\x00', 42U);
    RUN_MSG(test, '\x40', '\x42', '\x0F', 1000000U);
    RUN_MSG(test, '\xC0', '\xBD', '\xF0', 15777216U);
    RUN_MSG(test, '\xFF', '\x7F', '\x00', 32767U);
    RUN_MSG(test, '\xFF', '\xFF', '\x7F', 8388607U);
    RUN_MSG(test, '\xFF', '\xFF', '\xFF', 16777215U);
    RUN_MSG(test, '\x00', '\x00', '\x00', 0U);
}

void data_suite_int_uint32(TEST_TYPE (
    *test)(char const *msg, char c1, char c2, char c3, char c4, uint32_t val))
{
    RUN_MSG_INIT();
    RUN_MSG_FORMAT("0x%02hhx 0x%02hhx 0x%02hhx 0x%02hhx = %lu");
    RUN_MSG(test, '\x00', '\x00', '\x00', '\x80', 2147483648UL);
    RUN_MSG(test, '\x00', '\x01', '\x00', '\x00', 256U);
    RUN_MSG(test, '\x00', '\x2A', '\x00', '\x00', 10752U);
    RUN_MSG(test, '\x00', '\x80', '\xFF', '\xFF', 4294934528UL);
    RUN_MSG(test, '\x11', '\x0F', '\x00', '\x00', 3857U);
    RUN_MSG(test, '\x13', '\x0E', '\x16', '\x9D', 2635468307UL);
    RUN_MSG(test, '\x17', '\xFB', '\xFF', '\xFF', 4294966039UL);
    RUN_MSG(test, '\x2A', '\x00', '\x00', '\x00', 42U);
    RUN_MSG(test, '\x40', '\x42', '\x0F', '\x00', 1000000U);
    RUN_MSG(test, '\xC0', '\xBD', '\xF0', '\xFF', 4293967296UL);
    RUN_MSG(test, '\xED', '\x98', '\xD7', '\x34', 886544621U);
    RUN_MSG(test, '\xFF', '\x7F', '\x00', '\x00', 32767U);
    RUN_MSG(test, '\xFF', '\xFF', '\x7F', '\x00', 8388607U);
    RUN_MSG(test, '\xFF', '\xFF', '\xFF', '\x7F', 2147483647UL);
    RUN_MSG(test, '\xFF', '\xFF', '\xFF', '\xFF', 4294967295UL);
    RUN_MSG(test, '\x00', '\x00', '\x00', '\x00', 0U);
}
