#include "eaarlio/int_encode.h"
#include <stdint.h>

void eaarlio_int_encode_uint8(unsigned char *buf, uint8_t val)
{
    buf[0] = (unsigned char)val;
}

void eaarlio_int_encode_uint16(unsigned char *buf, uint16_t val)
{
    buf[0] = (unsigned char)(0xff & val);
    buf[1] = (unsigned char)(0xff & (val >> 8));
}

void eaarlio_int_encode_uint24(unsigned char *buf, uint32_t val)
{
    buf[0] = (unsigned char)(0xff & val);
    buf[1] = (unsigned char)(0xff & (val >> 8));
    buf[2] = (unsigned char)(0xff & (val >> 16));
}

void eaarlio_int_encode_uint32(unsigned char *buf, uint32_t val)
{
    buf[0] = (unsigned char)(0xff & val);
    buf[1] = (unsigned char)(0xff & (val >> 8));
    buf[2] = (unsigned char)(0xff & (val >> 16));
    buf[3] = (unsigned char)(0xff & (val >> 24));
}

void eaarlio_int_encode_int16(unsigned char *buf, int16_t val)
{
    eaarlio_int_encode_uint16(buf, (uint16_t)val);
}
