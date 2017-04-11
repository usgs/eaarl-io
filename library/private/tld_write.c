#include "eaarlio/tld.h"
#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/raster.h"
#include "eaarlio/stream_support.h"
#include "eaarlio/tld_constants.h"
#include "eaarlio/tld_encode.h"
#include "eaarlio/tld_pack.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

eaarlio_error eaarlio_tld_write_raster(struct eaarlio_stream *stream,
    struct eaarlio_raster *raster,
    struct eaarlio_memory *memory)
{
    eaarlio_error err = EAARLIO_SUCCESS;
    struct eaarlio_tld_header header;
    unsigned char *buf_raster;
    uint32_t buf_raster_len;
    unsigned char buf_record[EAARLIO_TLD_RECORD_HEADER_SIZE];

    if(!stream)
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

    err = eaarlio_tld_pack_raster(&buf_raster, &buf_raster_len, raster, memory);
    if(err != EAARLIO_SUCCESS)
        goto cleanup;

    header.record_type = EAARLIO_TLD_TYPE_RASTER;
    header.record_length = buf_raster_len + EAARLIO_TLD_RECORD_HEADER_SIZE;

    err = eaarlio_tld_encode_record_header(
        (unsigned char *)buf_record, EAARLIO_TLD_RECORD_HEADER_SIZE, &header);
    if(err != EAARLIO_SUCCESS)
        goto cleanup;

    err = stream->write(
        stream, EAARLIO_TLD_RECORD_HEADER_SIZE, (unsigned char *)buf_record);
    if(err != EAARLIO_SUCCESS)
        goto cleanup;

    err = stream->write(stream, buf_raster_len, buf_raster);

cleanup:
    if(buf_raster)
        memory->free(memory, buf_raster);

    return err;
}
