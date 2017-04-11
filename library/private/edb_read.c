#include "eaarlio/edb.h"
#include "eaarlio/edb_decode.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/edb_read.h"
#include "eaarlio/error.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/stream_support.h"
#include <assert.h>

eaarlio_error eaarlio_edb_read_header(struct eaarlio_stream *stream,
    struct eaarlio_edb_header *header)
{
    unsigned char buf[EAARLIO_EDB_HEADER_SIZE];
    eaarlio_error err;

    if(!header)
        return EAARLIO_NULL;

    header->files_offset = 0;
    header->record_count = 0;
    header->file_count = 0;

    if(!stream)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    err = stream->seek(stream, 0, SEEK_SET);
    if(err != EAARLIO_SUCCESS)
        return err;

    err = stream->read(stream, EAARLIO_EDB_HEADER_SIZE, (unsigned char *)&buf);
    if(err != EAARLIO_SUCCESS)
        return err;

    err = eaarlio_edb_decode_header(
        (unsigned char *)&buf, EAARLIO_EDB_HEADER_SIZE, header);
    assert(err == EAARLIO_SUCCESS);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_read_records(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    struct eaarlio_edb_record *records)
{
    unsigned char buf[EAARLIO_EDB_RECORD_SIZE];
    uint32_t i;
    eaarlio_error err;

    if(!stream)
        return EAARLIO_NULL;
    if(!header)
        return EAARLIO_NULL;
    if(!records)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    err = stream->seek(stream, EAARLIO_EDB_HEADER_SIZE, SEEK_SET);
    if(err != EAARLIO_SUCCESS)
        return err;

    for(i = 0; i < header->record_count; i++) {
        err = stream->read(
            stream, EAARLIO_EDB_RECORD_SIZE, (unsigned char *)&buf);
        if(err != EAARLIO_SUCCESS)
            return err;

        err = eaarlio_edb_decode_record(
            (unsigned char *)&buf, EAARLIO_EDB_RECORD_SIZE, &records[i]);
        assert(err == EAARLIO_SUCCESS);
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_read_filenames(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    char **filenames,
    struct eaarlio_memory *memory)
{
    eaarlio_error err;
    unsigned char *buf;
    uint32_t buf_len;
    uint16_t length;
    uint32_t i;

    if(!header)
        return EAARLIO_NULL;
    if(!filenames)
        return EAARLIO_NULL;
    for(i = 0; i < header->file_count; i++)
        filenames[i] = NULL;

    if(!stream)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    buf_len = 32;
    buf = memory->malloc(memory, buf_len);
    if(!buf)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    err = stream->seek(stream, header->files_offset, SEEK_SET);
    if(err != EAARLIO_SUCCESS)
        return err;

    for(i = 0; i < header->file_count; i++) {
        err = stream->read(stream, EAARLIO_EDB_FILENAME_LENGTH_SIZE, buf);
        if(err != EAARLIO_SUCCESS)
            goto cleanup;

        err = eaarlio_edb_decode_filename_length(buf, buf_len, &length);
        assert(err == EAARLIO_SUCCESS);

        if(length > buf_len + 1) {
            while(buf_len + 1 < length)
                buf_len *= 2;
            buf = memory->realloc(memory, buf, buf_len);
            if(!buf)
                return EAARLIO_MEMORY_ALLOC_FAIL;
        }

        err = stream->read(stream, length, buf);
        if(err != EAARLIO_SUCCESS)
            goto cleanup;

        filenames[i] = memory->malloc(memory, length + 1);
        if(!filenames[i]) {
            err = EAARLIO_MEMORY_ALLOC_FAIL;
            goto cleanup;
        }

        err = eaarlio_edb_decode_filename(
            buf, buf_len, length, filenames[i], length + 1);
        assert(err == EAARLIO_SUCCESS);

        if(length > 0)
            memcpy(filenames[i], buf, length);
        filenames[i][length] = 0;
    }

cleanup:
    if(buf)
        memory->free(memory, buf);

    return err;
}
