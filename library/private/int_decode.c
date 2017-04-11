#include "eaarlio/int_decode.h"
#include <stdint.h>

uint8_t eaarlio_int_decode_uint8(unsigned char const *const buf)
{
    return (uint8_t)buf[0];
}

uint16_t eaarlio_int_decode_uint16(unsigned char const *const buf)
{
    return (((uint16_t)buf[0]) | (((uint16_t)buf[1]) << 8));
}

uint32_t eaarlio_int_decode_uint24(unsigned char const *const buf)
{
    return (((uint32_t)buf[0]) | (((uint32_t)buf[1]) << 8)
        | (((uint32_t)buf[2]) << 16));
}

uint32_t eaarlio_int_decode_uint32(unsigned char const *const buf)
{
    return (((uint32_t)buf[0]) | (((uint32_t)buf[1]) << 8)
        | (((uint32_t)buf[2]) << 16) | (((uint32_t)buf[3]) << 24));
}

int16_t eaarlio_int_decode_int16(unsigned char const *const buf)
{
    return (int16_t)eaarlio_int_decode_uint16(buf);
}
