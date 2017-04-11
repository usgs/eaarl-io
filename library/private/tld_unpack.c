#include "eaarlio/tld.h"
#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_decode.h"
#include "eaarlio/tld_unpack.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

/* Helper to advance through the buffer by updated buffer and buffer_len.
 *
 * The functions below assume that the functions they are calling will return
 * an error when the buffer is short (i.e., that those functions are working
 * properly). An assertion in this function verifies that during testing.
 */
static void _eaarlio_advance_buffer(unsigned char const **buf,
    uint32_t *buf_len,
    uint16_t offset)
{
    assert(buf);
    assert(buf_len);
    assert(*buf_len >= offset);

    *buf += offset;
    *buf_len -= offset;
}

/* Wrapper around eaarlio_tld_decode_waveform that handles memory allocation.
 */
static eaarlio_error _eaarlio_retrieve_wf(unsigned char const *buffer,
    uint32_t buffer_len,
    unsigned char **wf,
    uint16_t wf_len,
    struct eaarlio_memory *memory)
{
    assert(buffer);
    assert(wf);

    if(wf_len < 1)
        return EAARLIO_SUCCESS;

    *wf = memory->calloc(memory, 1, wf_len);
    if(!*wf)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    return eaarlio_tld_decode_waveform(buffer, buffer_len, *wf, wf_len);
}

/* Decodes and assigns tx_len and tx
 */
static eaarlio_error _eaarlio_unpack_tx(unsigned char const **buffer,
    uint32_t *buffer_len,
    struct eaarlio_pulse *pulse,
    struct eaarlio_memory *mem)
{
    assert(buffer);
    assert(*buffer);
    assert(buffer_len);
    assert(pulse);

    eaarlio_error final = EAARLIO_SUCCESS;
    eaarlio_error err =
        eaarlio_tld_decode_tx_length(*buffer, *buffer_len, &pulse->tx_len);
    if(err != EAARLIO_SUCCESS)
        return err;
    _eaarlio_advance_buffer(buffer, buffer_len, EAARLIO_TLD_TX_LENGTH_SIZE);

    if(pulse->tx_len > *buffer_len) {
        pulse->tx_len = (uint8_t)*buffer_len;
        final = EAARLIO_BUFFER_SHORT;
    }

    err = _eaarlio_retrieve_wf(
        *buffer, *buffer_len, &pulse->tx, pulse->tx_len, mem);
    if(err != EAARLIO_SUCCESS)
        return err;
    _eaarlio_advance_buffer(buffer, buffer_len, pulse->tx_len);

    return final;
}

/* Decodes and assigns rx_len[channel] and rx[channel]
 */
static eaarlio_error _eaarlio_unpack_rx(unsigned char const **buffer,
    uint32_t *buffer_len,
    uint8_t channel,
    struct eaarlio_pulse *pulse,
    struct eaarlio_memory *mem)
{
    assert(buffer);
    assert(*buffer);
    assert(buffer_len);
    assert(pulse);

    eaarlio_error final = EAARLIO_SUCCESS;
    eaarlio_error err = eaarlio_tld_decode_rx_length(
        *buffer, *buffer_len, &pulse->rx_len[channel]);
    if(err != EAARLIO_SUCCESS)
        return err;
    _eaarlio_advance_buffer(buffer, buffer_len, EAARLIO_TLD_RX_LENGTH_SIZE);

    if(pulse->rx_len[channel] > *buffer_len) {
        pulse->rx_len[channel] = (uint16_t)*buffer_len;
        final = EAARLIO_BUFFER_SHORT;
    }

    err = _eaarlio_retrieve_wf(
        *buffer, *buffer_len, &pulse->rx[channel], pulse->rx_len[channel], mem);
    if(err != EAARLIO_SUCCESS)
        return err;
    _eaarlio_advance_buffer(buffer, buffer_len, pulse->rx_len[channel]);

    return final;
}

eaarlio_error eaarlio_tld_unpack_waveforms(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_pulse *pulse,
    struct eaarlio_memory *memory)
{
    eaarlio_error err;
    uint8_t channel;

    if(!pulse)
        return EAARLIO_NULL;

    /* If we exit early due to an error, we don't want there to be any
     * ambiguity over whether a pointer is valid or not. Setting them all NULL
     * up front helps to prevent wild/dangling pointers.
     */
    pulse->tx = NULL;
    pulse->rx[0] = NULL;
    pulse->rx[1] = NULL;
    pulse->rx[2] = NULL;
    pulse->rx[3] = NULL;

    if(!buffer)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    err = _eaarlio_unpack_tx(&buffer, &buffer_len, pulse, memory);
    if(err != EAARLIO_SUCCESS)
        return err;

    for(channel = 0; channel < pulse->rx_count; channel++) {
        err = _eaarlio_unpack_rx(&buffer, &buffer_len, channel, pulse, memory);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_unpack_pulses(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_waveforms)
{
    eaarlio_error err;
    uint16_t data_length;
    uint8_t i, j;

    if(!buffer)
        return EAARLIO_NULL;
    if(!raster)
        return EAARLIO_NULL;

    if(raster->pulse_count < 1) {
        raster->pulse = NULL;
        return EAARLIO_SUCCESS;
    }

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    raster->pulse = memory->calloc(
        memory, raster->pulse_count, sizeof(struct eaarlio_pulse));
    if(!raster->pulse)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    for(i = 0; i < raster->pulse_count; i++) {
        err = eaarlio_tld_decode_pulse_header(
            buffer, buffer_len, &raster->pulse[i]);
        if(err != EAARLIO_SUCCESS)
            return err;
        _eaarlio_advance_buffer(
            &buffer, &buffer_len, EAARLIO_TLD_PULSE_HEADER_SIZE);

        err =
            eaarlio_tld_decode_wf_data_length(buffer, buffer_len, &data_length);
        if(err != EAARLIO_SUCCESS)
            return err;
        _eaarlio_advance_buffer(
            &buffer, &buffer_len, EAARLIO_TLD_WF_DATA_LENGTH_SIZE);

        if(data_length > buffer_len)
            data_length = (uint16_t)buffer_len;

        if(include_waveforms) {
            err = eaarlio_tld_unpack_waveforms(
                buffer, data_length, &raster->pulse[i], memory);

            /* Special case: The EAARL system is known to write out the last
             * waveform of the last pulse of a raster incorrectly by shorting
             * it two bytes. So if we get EAARLIO_BUFFER_SHORT on the last
             * pulse, then it's not an error if all expected waveforms are
             * present.
             */
            if(err == EAARLIO_BUFFER_SHORT) {
                if(i + 1 < raster->pulse_count)
                    return err;
                if(!raster->pulse[i].tx)
                    return err;
                j = raster->pulse[i].rx_count;
                if(j > EAARLIO_MAX_RX_COUNT)
                    j = EAARLIO_MAX_RX_COUNT;
                while(j--)
                    if(!raster->pulse[i].rx[j])
                        return err;
            } else if(err != EAARLIO_SUCCESS) {
                return err;
            }
        } else {
            raster->pulse[i].tx = NULL;
            raster->pulse[i].rx[0] = NULL;
            raster->pulse[i].rx[1] = NULL;
            raster->pulse[i].rx[2] = NULL;
            raster->pulse[i].rx[3] = NULL;
        }
        _eaarlio_advance_buffer(&buffer, &buffer_len, data_length);
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_tld_unpack_raster(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_pulses,
    int include_waveforms)
{
    eaarlio_error err;

    if(!buffer)
        return EAARLIO_NULL;
    if(!raster)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    /* If we exit early due to an error, we don't want there to be any
     * ambiguity over whether a pointer is valid or not. Setting it to NULL up
     * front helps to prevent wild/dangling pointers.
     */
    raster->pulse = NULL;

    err = eaarlio_tld_decode_raster_header(buffer, buffer_len, raster);
    if(err != EAARLIO_SUCCESS)
        return err;
    if(!include_pulses)
        return EAARLIO_SUCCESS;

    _eaarlio_advance_buffer(
        &buffer, &buffer_len, EAARLIO_TLD_RASTER_HEADER_SIZE);

    return eaarlio_tld_unpack_pulses(
        buffer, buffer_len, raster, memory, include_waveforms);
}
