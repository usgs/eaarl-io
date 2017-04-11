#include "eaarlio/tld.h"
#include "eaarlio/error.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_size.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

/**
 * The C specification states that UINTn_MAX macros are supposed to use the
 * type that they correspond to, but gcc fails to enforce this so UINT16_MAX
 * can end up signed instead of unsigned on some platforms.
 */
#ifdef UINT16_MAX
#undef UINT16_MAX
#endif
#define UINT16_MAX 65535U

#ifndef UINT24_MAX
#define UINT24_MAX 16777215U
#endif

eaarlio_error eaarlio_tld_size_pulse_waveforms(
    struct eaarlio_pulse const *pulse,
    uint16_t *size)
{
    uint16_t _size = 0U;
    uint8_t rx_count, channel;

    if(!size)
        return EAARLIO_NULL;
    *size = 0U;
    if(!pulse)
        return EAARLIO_NULL;

    _size = EAARLIO_TLD_TX_LENGTH_SIZE + pulse->tx_len;

    rx_count = pulse->rx_count;
    if(rx_count > EAARLIO_MAX_RX_COUNT) {
        rx_count = EAARLIO_MAX_RX_COUNT;
    }

    for(channel = 0; channel < rx_count; channel++) {
        if(_size >= (UINT16_MAX - EAARLIO_TLD_RX_LENGTH_SIZE)) {
            return EAARLIO_VALUE_OUT_OF_RANGE;
        }
        _size += EAARLIO_TLD_RX_LENGTH_SIZE;
        if(_size >= (UINT16_MAX - pulse->rx_len[channel])) {
            return EAARLIO_VALUE_OUT_OF_RANGE;
        }
        _size += pulse->rx_len[channel];
    }

    *size = _size;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_size_pulse(struct eaarlio_pulse const *pulse,
    uint32_t *size)
{
    uint16_t size_waveforms = 0U;
    eaarlio_error err;

    if(!size)
        return EAARLIO_NULL;
    *size = 0U;
    if(!pulse)
        return EAARLIO_NULL;

    err = eaarlio_tld_size_pulse_waveforms(pulse, &size_waveforms);
    if(err != EAARLIO_SUCCESS)
        return err;

    /* It's impossible to overflow 24-bit since size_waveforms is 16-bit and
     * the constants total is 8-bit, so no bounds check is performed.
     */

    *size = EAARLIO_TLD_PULSE_HEADER_SIZE + EAARLIO_TLD_WF_DATA_LENGTH_SIZE
        + size_waveforms;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_size_raster_pulses(
    struct eaarlio_raster const *raster,
    uint32_t *size)
{
    uint32_t _size = 0U;
    uint32_t pulse_size = 0U;
    eaarlio_error err;
    uint16_t i;

    if(!size)
        return EAARLIO_NULL;
    *size = 0U;
    if(!raster)
        return EAARLIO_NULL;

    if(raster->pulse_count == 0)
        return EAARLIO_SUCCESS;
    if(!raster->pulse)
        return EAARLIO_NULL;

    for(i = 0; i < raster->pulse_count; i++) {
        err = eaarlio_tld_size_pulse(&raster->pulse[i], &pulse_size);
        if(err != EAARLIO_SUCCESS)
            return err;

        if(_size >= (UINT24_MAX - pulse_size)) {
            return EAARLIO_VALUE_OUT_OF_RANGE;
        }

        _size += pulse_size;
    }

    *size = _size;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_size_raster(struct eaarlio_raster const *raster,
    uint32_t *size)
{
    uint32_t pulses_size = 0U;
    eaarlio_error err;

    if(!size)
        return EAARLIO_NULL;
    *size = 0U;
    if(!raster)
        return EAARLIO_NULL;

    err = eaarlio_tld_size_raster_pulses(raster, &pulses_size);
    if(err != EAARLIO_SUCCESS)
        return err;
    if(pulses_size > (UINT24_MAX - EAARLIO_TLD_RASTER_HEADER_SIZE)) {
        return EAARLIO_VALUE_OUT_OF_RANGE;
    }

    *size = EAARLIO_TLD_RASTER_HEADER_SIZE + pulses_size;
    return EAARLIO_SUCCESS;
}
