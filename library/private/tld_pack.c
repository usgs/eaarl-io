#include "eaarlio/tld.h"
#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/raster.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_encode.h"
#include "eaarlio/tld_pack.h"
#include "eaarlio/tld_size.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

eaarlio_error eaarlio_tld_pack_raster(unsigned char **buffer,
    uint32_t *buffer_len,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory)
{
    struct eaarlio_pulse *pulse;
    eaarlio_error err;
    unsigned char *work;
    uint32_t work_len;
    uint16_t wfdata_len;
    uint16_t i;
    uint8_t j;

    if(buffer)
        *buffer = NULL;
    if(buffer_len)
        *buffer_len = 0;

    if(!buffer)
        return EAARLIO_NULL;
    if(!buffer_len)
        return EAARLIO_NULL;
    if(!raster)
        return EAARLIO_NULL;
    if(!raster->pulse && raster->pulse_count > 0)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    err = eaarlio_tld_size_raster(raster, buffer_len);
    if(err != EAARLIO_SUCCESS)
        return err;

    *buffer = memory->calloc(memory, 1, *buffer_len);
    if(!buffer)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    work = *buffer;
    work_len = *buffer_len;

#define CHECK_AND_ADVANCE(size)                                                \
    do {                                                                       \
        if(err != EAARLIO_SUCCESS)                                             \
            return err;                                                        \
        work += (size);                                                        \
        work_len -= (size);                                                    \
    } while(0)

    err = eaarlio_tld_encode_raster_header(work, work_len, raster);
    CHECK_AND_ADVANCE(EAARLIO_TLD_RASTER_HEADER_SIZE);

    for(i = 0; i < raster->pulse_count; i++) {
        pulse = &raster->pulse[i];

        err = eaarlio_tld_encode_pulse_header(work, work_len, pulse);
        CHECK_AND_ADVANCE(EAARLIO_TLD_PULSE_HEADER_SIZE);

        err = eaarlio_tld_size_pulse_waveforms(pulse, &wfdata_len);
        if(err != EAARLIO_SUCCESS)
            return err;

        err = eaarlio_tld_encode_wf_data_length(work, work_len, wfdata_len);
        CHECK_AND_ADVANCE(EAARLIO_TLD_WF_DATA_LENGTH_SIZE);

        err = eaarlio_tld_encode_tx_length(work, work_len, pulse->tx_len);
        CHECK_AND_ADVANCE(EAARLIO_TLD_TX_LENGTH_SIZE);

        err = eaarlio_tld_encode_waveform(
            work, work_len, pulse->tx, pulse->tx_len);
        CHECK_AND_ADVANCE(pulse->tx_len);

        for(j = 0; j < pulse->rx_count; j++) {
            err =
                eaarlio_tld_encode_rx_length(work, work_len, pulse->rx_len[j]);
            CHECK_AND_ADVANCE(EAARLIO_TLD_RX_LENGTH_SIZE);

            err = eaarlio_tld_encode_waveform(
                work, work_len, pulse->rx[j], pulse->rx_len[j]);
            CHECK_AND_ADVANCE(pulse->rx_len[j]);
        }
    }

#undef CHECK_AND_ADVANCE

    return EAARLIO_SUCCESS;
}
