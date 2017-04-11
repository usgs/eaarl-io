#include "eaarlio/flight.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/stream_support.h"
#include "eaarlio/tld.h"

/**
 * Internal state for ::eaarlio_flight::internal
 */
struct _eaarlio_flight_internal {
    /** Stream for currently open TLD */
    struct eaarlio_stream stream;
    /** Memory handler */
    struct eaarlio_memory *memory;
    /** ::eaarlio_edb_record::file_index corresponding to #stream */
    int16_t file_index;
};

eaarlio_error eaarlio_flight_init(struct eaarlio_flight *flight,
    struct eaarlio_memory *memory)
{
    struct _eaarlio_flight_internal *internal;

    if(!flight)
        return EAARLIO_NULL;
    if(flight->internal)
        return EAARLIO_FLIGHT_INVALID;
    flight->internal = NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    internal = memory->malloc(memory, sizeof(struct _eaarlio_flight_internal));
    if(!internal)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    internal->memory = memory;
    internal->stream = eaarlio_stream_empty();
    internal->file_index = 0;
    flight->internal = internal;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_flight_free(struct eaarlio_flight *flight)
{
    struct _eaarlio_flight_internal *internal;
    eaarlio_error err;

    if(!flight)
        return EAARLIO_NULL;
    if(!flight->internal)
        return EAARLIO_FLIGHT_INVALID;

    internal = (struct _eaarlio_flight_internal *)flight->internal;

    if(internal->stream.close) {
        err = internal->stream.close(&internal->stream);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    if(flight->tld_opener.close) {
        err = flight->tld_opener.close(&flight->tld_opener);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    err = eaarlio_edb_free(&flight->edb, internal->memory);
    if(err != EAARLIO_SUCCESS)
        return err;

    internal->memory->free(internal->memory, internal);
    flight->internal = NULL;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_flight_read_raster(struct eaarlio_flight *flight,
    struct eaarlio_raster *raster,
    int32_t *time_offset,
    uint32_t raster_number,
    int include_pulses,
    int include_waveforms)
{
    struct _eaarlio_flight_internal *internal;
    struct eaarlio_stream *stream;
    struct eaarlio_edb_record record;
    eaarlio_error err;

    if(raster)
        *raster = eaarlio_raster_empty();
    if(time_offset)
        *time_offset = 0;

    if(!flight)
        return EAARLIO_NULL;
    if(!raster)
        return EAARLIO_NULL;
    if(!flight->tld_opener.close)
        return EAARLIO_TLD_OPENER_INVALID;
    if(!flight->tld_opener.open_tld)
        return EAARLIO_TLD_OPENER_INVALID;
    if(!flight->edb.records)
        return EAARLIO_FLIGHT_INVALID;
    if(!flight->edb.files)
        return EAARLIO_FLIGHT_INVALID;
    if(!flight->internal)
        return EAARLIO_FLIGHT_INVALID;

    internal = (struct _eaarlio_flight_internal *)flight->internal;
    stream = &internal->stream;

    if(raster_number > flight->edb.record_count)
        return EAARLIO_FLIGHT_RASTER_INVALID;
    record = flight->edb.records[raster_number - 1];
    if(record.file_index < 1)
        return EAARLIO_CORRUPT;
    if((uint32_t)record.file_index > flight->edb.file_count)
        return EAARLIO_CORRUPT;

    if(internal->file_index && !eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    if(internal->file_index && internal->file_index != record.file_index) {
        err = stream->close(stream);
        if(err != EAARLIO_SUCCESS)
            return err;
        internal->file_index = 0;
    }

    if(internal->file_index != record.file_index) {
        err = flight->tld_opener.open_tld(&flight->tld_opener, stream,
            flight->edb.files[record.file_index - 1]);
        if(err != EAARLIO_SUCCESS)
            return err;
        internal->file_index = record.file_index;
    }

    err = stream->seek(stream, record.record_offset, SEEK_SET);
    if(err != EAARLIO_SUCCESS)
        return err;

    err = eaarlio_tld_read_raster(
        stream, raster, internal->memory, include_pulses, include_waveforms);
    if(err != EAARLIO_SUCCESS)
        return err;

    if(time_offset) {
        *time_offset = record.time_seconds - raster->time_seconds;
    }

    return EAARLIO_SUCCESS;
}
