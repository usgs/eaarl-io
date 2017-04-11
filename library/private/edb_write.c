#include "eaarlio/edb.h"
#include "eaarlio/edb_encode.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/edb_write.h"
#include "eaarlio/error.h"
#include "eaarlio/misc_support.h"
#include "eaarlio/stream_support.h"
#include <assert.h>

eaarlio_error eaarlio_edb_write_header(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header)
{
    unsigned char buf[EAARLIO_EDB_HEADER_SIZE];
    eaarlio_error err;

    if(!stream)
        return EAARLIO_NULL;
    if(!header)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    err = eaarlio_edb_encode_header(
        (unsigned char *)buf, EAARLIO_EDB_HEADER_SIZE, header);
    assert(err == EAARLIO_SUCCESS);

    err = stream->seek(stream, 0, SEEK_SET);
    if(err != EAARLIO_SUCCESS)
        return err;

    return stream->write(stream, EAARLIO_EDB_HEADER_SIZE, (unsigned char *)buf);
}

eaarlio_error eaarlio_edb_write_records(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    struct eaarlio_edb_record const *records)
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
        // Encode record to buffer
        err = eaarlio_edb_encode_record(
            (unsigned char *)&buf, EAARLIO_EDB_RECORD_SIZE, &records[i]);
        assert(err == EAARLIO_SUCCESS);

        // Write buffer to file
        err = stream->write(
            stream, EAARLIO_EDB_RECORD_SIZE, (unsigned char *)&buf);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_write_filenames(struct eaarlio_stream *stream,
    struct eaarlio_edb_header const *header,
    char const *const *filenames)
{
    eaarlio_error err;
    uint32_t i;
    size_t len;
    unsigned char buf[EAARLIO_EDB_FILENAME_LENGTH_SIZE];

    if(!stream)
        return EAARLIO_NULL;
    if(!header)
        return EAARLIO_NULL;
    if(!filenames)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    err = stream->seek(stream, header->files_offset, SEEK_SET);
    if(err != EAARLIO_SUCCESS)
        return err;

    for(i = 0; i < header->file_count; i++) {
        // Get length of the current filename and make sure it's not too long
        len =
            eaarlio_strnlen(filenames[i], EAARLIO_EDB_FILENAME_MAX_LENGTH + 1);
        if(len > EAARLIO_EDB_FILENAME_MAX_LENGTH) {
            return EAARLIO_EDB_FILENAME_TOO_LONG;
        }

        // Write the length of the filename
        err = eaarlio_edb_encode_filename_length((unsigned char *)buf,
            EAARLIO_EDB_FILENAME_LENGTH_SIZE, (uint16_t)len);
        assert(err == EAARLIO_SUCCESS);
        err = stream->write(
            stream, EAARLIO_EDB_FILENAME_LENGTH_SIZE, (unsigned char *)buf);
        if(err != EAARLIO_SUCCESS)
            return err;

        // Write the filename. Omit the null terminator.
        err = stream->write(stream, len, (unsigned char *)filenames[i]);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    return EAARLIO_SUCCESS;
}
