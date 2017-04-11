#include "eaarlio/edb.h"
#include "eaarlio/edb_encode.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/error.h"
#include "eaarlio/int_encode.h"
#include "eaarlio/misc_support.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

eaarlio_error eaarlio_edb_encode_header(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_header const *header)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!header)
        return EAARLIO_NULL;
    if(buffer_len < EAARLIO_EDB_HEADER_SIZE)
        return EAARLIO_BUFFER_SHORT;

    eaarlio_int_encode_uint32(buffer, header->files_offset);
    buffer += 4;

    eaarlio_int_encode_uint32(buffer, header->record_count);
    buffer += 4;

    eaarlio_int_encode_uint32(buffer, header->file_count);

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_encode_record(unsigned char *buffer,
    uint32_t buffer_len,
    struct eaarlio_edb_record const *record)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(!record)
        return EAARLIO_NULL;
    if(buffer_len < EAARLIO_EDB_RECORD_SIZE)
        return EAARLIO_BUFFER_SHORT;

    eaarlio_int_encode_uint32(buffer, record->time_seconds);
    buffer += 4;

    eaarlio_int_encode_uint32(buffer, record->time_fraction);
    buffer += 4;

    eaarlio_int_encode_uint32(buffer, record->record_offset);
    buffer += 4;

    eaarlio_int_encode_uint32(buffer, record->record_length);
    buffer += 4;

    eaarlio_int_encode_int16(buffer, record->file_index);
    buffer += 2;

    eaarlio_int_encode_uint8(buffer, record->pulse_count);
    buffer += 1;

    eaarlio_int_encode_uint8(buffer, record->digitizer);

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_encode_filename_length(unsigned char *buffer,
    uint32_t buffer_len,
    uint16_t length)
{
    if(!buffer)
        return EAARLIO_NULL;
    if(buffer_len < EAARLIO_EDB_FILENAME_LENGTH_SIZE)
        return EAARLIO_BUFFER_SHORT;
    eaarlio_int_encode_uint16(buffer, length);
    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_encode_filename(unsigned char *buffer,
    uint32_t buffer_len,
    char const *filename)
{
    size_t len;
    if(!buffer)
        return EAARLIO_NULL;
    len = eaarlio_strnlen(filename, EAARLIO_EDB_FILENAME_MAX_LENGTH + 1);
    if(len > EAARLIO_EDB_FILENAME_MAX_LENGTH)
        return EAARLIO_EDB_FILENAME_TOO_LONG;
    if(buffer_len < len)
        return EAARLIO_BUFFER_SHORT;
    memcpy(buffer, filename, len);
    return EAARLIO_SUCCESS;
}
