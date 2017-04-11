#include "eaarlio/tld.h"
#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/raster.h"
#include "eaarlio/stream_support.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_decode.h"
#include "eaarlio/tld_unpack.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

eaarlio_error eaarlio_tld_read_record(struct eaarlio_stream *stream,
    struct eaarlio_tld_header *record_header,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_pulses,
    int include_waveforms)
{
    eaarlio_error err;
    unsigned char *buf;
    int32_t raster_length;
    uint32_t buf_len;

    if(record_header) {
        record_header->record_length = 0;
        record_header->record_type = 0;
    }

    if(raster) {
        raster->pulse = NULL;
    }

    if(!stream)
        return EAARLIO_NULL;
    if(!record_header)
        return EAARLIO_NULL;
    if(!raster)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    buf_len = EAARLIO_TLD_RECORD_HEADER_SIZE;
    buf = memory->malloc(memory, buf_len);
    if(!buf)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    err = stream->read(stream, buf_len, buf);
    if(err != EAARLIO_SUCCESS)
        goto cleanup;

    err = eaarlio_tld_decode_record_header(buf, buf_len, record_header);
    if(err != EAARLIO_SUCCESS)
        goto cleanup;

    raster_length =
        record_header->record_length - EAARLIO_TLD_RECORD_HEADER_SIZE;
    if(raster_length < 0) {
        err = EAARLIO_CORRUPT;
        goto cleanup;
    }

    if(record_header->record_type != EAARLIO_TLD_TYPE_RASTER) {
        if(raster_length > 0)
            stream->seek(stream, raster_length, SEEK_CUR);
        err = EAARLIO_SUCCESS;
        goto cleanup;
    }

    buf_len =
        include_pulses ? (size_t)raster_length : EAARLIO_TLD_RASTER_HEADER_SIZE;

    buf = memory->realloc(memory, buf, buf_len);
    if(!buf)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    err = stream->read(stream, buf_len, buf);
    if(err != EAARLIO_SUCCESS)
        goto cleanup;

    if(buf_len < (size_t)raster_length) {
        err = stream->seek(stream, raster_length - buf_len, SEEK_CUR);
        if(err != EAARLIO_SUCCESS)
            goto cleanup;
    }

    err = eaarlio_tld_unpack_raster(buf, (uint32_t)raster_length, raster,
        memory, include_pulses, include_waveforms);

cleanup:
    if(buf)
        memory->free(memory, buf);
    return err;
}

eaarlio_error eaarlio_tld_read_raster(struct eaarlio_stream *stream,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory,
    int include_pulses,
    int include_waveforms)
{
    struct eaarlio_tld_header record_header;
    eaarlio_error err;

    err = eaarlio_tld_read_record(stream, &record_header, raster, memory,
        include_pulses, include_waveforms);
    if(err != EAARLIO_SUCCESS)
        return err;

    if(record_header.record_type != EAARLIO_TLD_TYPE_RASTER)
        return EAARLIO_TLD_TYPE_UNKNOWN;

    return EAARLIO_SUCCESS;
}
