#include "eaarlio/tld.h"
#include "eaarlio/error.h"
#include "eaarlio/int_decode.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld_decode.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

eaarlio_error eaarlio_tld_decode_record_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_tld_header *record_header)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!record_header)
        return EAARLIO_NULL;

    if(buffer_len < 4) {
        return EAARLIO_BUFFER_SHORT;
    }

    record_header->record_length = eaarlio_int_decode_uint24(buffer);
    buffer += 3;

    record_header->record_type = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_decode_raster_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster *raster)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!raster)
        return EAARLIO_NULL;

    if(buffer_len < 14) {
        return EAARLIO_BUFFER_SHORT;
    }

    raster->time_seconds = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    raster->time_fraction = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    raster->sequence_number = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    {
        uint16_t tmp = eaarlio_int_decode_uint16(buffer);
        buffer += 2;

        raster->pulse_count = tmp & 0x7fff;
        raster->digitizer = (tmp >> 15) & 0x1;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_decode_pulse_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_pulse *pulse)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!pulse)
        return EAARLIO_NULL;
    if(buffer_len < 13)
        return EAARLIO_BUFFER_SHORT;

    pulse->time_offset = eaarlio_int_decode_uint24(buffer);
    buffer += 3;

    pulse->rx_count = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    pulse->bias_tx = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    pulse->bias_rx[0] = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    pulse->bias_rx[1] = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    pulse->bias_rx[2] = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    pulse->bias_rx[3] = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    pulse->scan_angle_counts = eaarlio_int_decode_int16(buffer);
    buffer += 2;

    {
        uint16_t tmp = eaarlio_int_decode_uint16(buffer);
        buffer += 2;

        pulse->range = (tmp & 0x3fff);
        pulse->thresh_tx = (tmp & 0x4000) >> 14;
        pulse->thresh_rx = (tmp & 0x8000) >> 15;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_decode_wf_data_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t *length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!length)
        return EAARLIO_NULL;
    if(buffer_len < 2)
        return EAARLIO_BUFFER_SHORT;
    *length = eaarlio_int_decode_uint16(buffer);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_decode_tx_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint8_t *length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!length)
        return EAARLIO_NULL;
    if(buffer_len < 1)
        return EAARLIO_BUFFER_SHORT;
    *length = eaarlio_int_decode_uint8(buffer);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_decode_rx_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t *length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!length)
        return EAARLIO_NULL;
    if(buffer_len < 2)
        return EAARLIO_BUFFER_SHORT;
    *length = eaarlio_int_decode_uint16(buffer);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_decode_waveform(unsigned char const *buffer,
    uint32_t buffer_len,
    unsigned char *waveform,
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
    memcpy(waveform, buffer, waveform_len);
    return EAARLIO_SUCCESS;
}
