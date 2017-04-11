#include "eaarlio/tld.h"
#include "eaarlio/error.h"
#include "eaarlio/int_encode.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld_encode.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

eaarlio_error eaarlio_tld_encode_record_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_tld_header const *record_header)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!record_header)
        return EAARLIO_NULL;
    if(buffer_len < 4)
        return EAARLIO_BUFFER_SHORT;
    if(record_header->record_length > 16777215)
        return EAARLIO_VALUE_OUT_OF_RANGE;

    eaarlio_int_encode_uint24(buffer, record_header->record_length);
    buffer += 3;

    eaarlio_int_encode_uint8(buffer, record_header->record_type);
    buffer += 1;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_encode_raster_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster const *raster)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!raster)
        return EAARLIO_NULL;
    if(buffer_len < 14)
        return EAARLIO_BUFFER_SHORT;
    if(raster->pulse_count > 32767)
        return EAARLIO_VALUE_OUT_OF_RANGE;
    if(raster->digitizer > 1)
        return EAARLIO_VALUE_OUT_OF_RANGE;

    eaarlio_int_encode_uint32(buffer, raster->time_seconds);
    buffer += 4;

    eaarlio_int_encode_uint32(buffer, raster->time_fraction);
    buffer += 4;

    eaarlio_int_encode_uint32(buffer, raster->sequence_number);
    buffer += 4;

    {
        uint16_t tmp = 0;
        tmp = tmp | (raster->pulse_count & 0x7fffU);
        tmp = tmp | ((raster->digitizer & 0x01U) << 15);
        eaarlio_int_encode_uint16(buffer, tmp);
        buffer += 2;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_encode_pulse_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_pulse const *pulse)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!pulse)
        return EAARLIO_NULL;
    if(buffer_len < 13)
        return EAARLIO_BUFFER_SHORT;
    if(pulse->range > 16383)
        return EAARLIO_VALUE_OUT_OF_RANGE;
    if(pulse->thresh_tx > 1)
        return EAARLIO_VALUE_OUT_OF_RANGE;
    if(pulse->thresh_rx > 1)
        return EAARLIO_VALUE_OUT_OF_RANGE;

    eaarlio_int_encode_uint24(buffer, pulse->time_offset);
    buffer += 3;

    eaarlio_int_encode_uint8(buffer, pulse->rx_count);
    buffer += 1;

    eaarlio_int_encode_uint8(buffer, pulse->bias_tx);
    buffer += 1;

    eaarlio_int_encode_uint8(buffer, pulse->bias_rx[0]);
    buffer += 1;

    eaarlio_int_encode_uint8(buffer, pulse->bias_rx[1]);
    buffer += 1;

    eaarlio_int_encode_uint8(buffer, pulse->bias_rx[2]);
    buffer += 1;

    eaarlio_int_encode_uint8(buffer, pulse->bias_rx[3]);
    buffer += 1;

    eaarlio_int_encode_int16(buffer, pulse->scan_angle_counts);
    buffer += 2;

    {
        uint16_t tmp = 0;

        tmp |= (pulse->range & 0x3fff);
        tmp |= ((pulse->thresh_tx & 0x01) << 14);
        tmp |= ((pulse->thresh_rx & 0x01) << 15);

        eaarlio_int_encode_uint16(buffer, tmp);
        buffer += 2;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_encode_wf_data_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint16_t length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(buffer_len < 2)
        return EAARLIO_BUFFER_SHORT;
    eaarlio_int_encode_uint16(buffer, length);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_encode_tx_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint8_t length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(buffer_len < 1)
        return EAARLIO_BUFFER_SHORT;
    eaarlio_int_encode_uint8(buffer, length);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_encode_rx_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint16_t length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(buffer_len < 2)
        return EAARLIO_BUFFER_SHORT;
    eaarlio_int_encode_uint16(buffer, length);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_encode_waveform(unsigned char *buffer,
    uint32_t buffer_len,
    unsigned char const *waveform,
    uint16_t waveform_len)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!waveform)
        return EAARLIO_NULL;
    if(buffer_len < waveform_len)
        return EAARLIO_BUFFER_SHORT;
    if(waveform_len == 0)
        return EAARLIO_SUCCESS;
    memcpy(buffer, waveform, waveform_len);
    return EAARLIO_SUCCESS;
}
