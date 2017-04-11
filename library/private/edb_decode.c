#include "eaarlio/edb.h"
#include "eaarlio/edb_decode.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/error.h"
#include "eaarlio/int_decode.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

eaarlio_error eaarlio_edb_decode_header(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_header *header)
{
    if(!header)
        return EAARLIO_NULL;
    if(!buffer)
        return EAARLIO_NULL;
    if(buffer_len < EAARLIO_EDB_HEADER_SIZE)
        return EAARLIO_BUFFER_SHORT;

    header->files_offset = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    header->record_count = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    header->file_count = eaarlio_int_decode_uint32(buffer);

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_decode_record(unsigned char const *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_record *record)
{
    if(!record)
        return EAARLIO_NULL;
    if(!buffer)
        return EAARLIO_NULL;
    if(buffer_len < EAARLIO_EDB_RECORD_SIZE)
        return EAARLIO_BUFFER_SHORT;

    record->time_seconds = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    record->time_fraction = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    record->record_offset = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    record->record_length = eaarlio_int_decode_uint32(buffer);
    buffer += 4;

    record->file_index = eaarlio_int_decode_int16(buffer);
    buffer += 2;

    record->pulse_count = eaarlio_int_decode_uint8(buffer);
    buffer += 1;

    record->digitizer = eaarlio_int_decode_uint8(buffer);

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_decode_filename_length(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t *length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!length)
        return EAARLIO_NULL;
    if(buffer_len < EAARLIO_EDB_FILENAME_LENGTH_SIZE)
        return EAARLIO_BUFFER_SHORT;
    *length = eaarlio_int_decode_uint16(buffer);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_decode_filename(unsigned char const *buffer,
    uint32_t buffer_len,
    uint16_t length,
    char *filename,
    size_t filename_len)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!filename)
        return EAARLIO_NULL;
    if(length == 0)
        return EAARLIO_SUCCESS;
    if(buffer_len < length)
        return EAARLIO_BUFFER_SHORT;
    if(filename_len <= length)
        return EAARLIO_BUFFER_SHORT;
    memcpy(filename, buffer, length);
    filename[length] = 0;
    return EAARLIO_SUCCESS;
}
