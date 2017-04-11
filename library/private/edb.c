#include "eaarlio/edb.h"
#include "eaarlio/edb_internals.h"
#include "eaarlio/edb_read.h"
#include "eaarlio/edb_write.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/stream_support.h"

eaarlio_error eaarlio_edb_free(struct eaarlio_edb *edb,
    struct eaarlio_memory *memory)
{
    uint32_t i;

    if(!edb)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    if(edb->records) {
        memory->free(memory, edb->records);
        edb->records = NULL;
    }

    if(edb->files) {
        for(i = 0; i < edb->file_count; i++) {
            if(edb->files[i]) {
                memory->free(memory, edb->files[i]);
                edb->files[i] = NULL;
            }
        }

        memory->free(memory, edb->files);
        edb->files = NULL;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_read(struct eaarlio_stream *stream,
    struct eaarlio_edb *edb,
    struct eaarlio_memory *memory,
    int include_records,
    int include_files)
{
    eaarlio_error err;
    struct eaarlio_edb_header header;

    if(edb) {
        edb->record_count = 0;
        edb->file_count = 0;
        edb->records = NULL;
        edb->files = NULL;
    }

    if(!stream)
        return EAARLIO_NULL;
    if(!edb)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    err = eaarlio_edb_read_header(stream, &header);
    if(err != EAARLIO_SUCCESS)
        return err;

    edb->record_count = header.record_count;
    edb->file_count = header.file_count;

    if(include_records && header.record_count > 0) {
        edb->records = (struct eaarlio_edb_record *)memory->malloc(
            memory, header.record_count * sizeof(struct eaarlio_edb_record));
        if(!edb->records)
            return EAARLIO_MEMORY_ALLOC_FAIL;
        err = eaarlio_edb_read_records(stream, &header, edb->records);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    if(include_files && header.file_count > 0) {
        edb->files =
            (char **)memory->malloc(memory, header.file_count * sizeof(char *));
        if(!edb->files)
            return EAARLIO_MEMORY_ALLOC_FAIL;
        err = eaarlio_edb_read_filenames(stream, &header, edb->files, memory);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_edb_write(struct eaarlio_stream *stream,
    struct eaarlio_edb const *edb)
{
    eaarlio_error err;
    struct eaarlio_edb_header header;

    if(!stream)
        return EAARLIO_NULL;
    if(!edb)
        return EAARLIO_NULL;
    if(edb->record_count > 0 && !edb->records)
        return EAARLIO_NULL;
    if(edb->file_count > 0 && !edb->files)
        return EAARLIO_NULL;

    if(!eaarlio_stream_valid(stream))
        return EAARLIO_STREAM_INVALID;

    header.record_count = edb->record_count;
    header.file_count = edb->file_count;
    header.files_offset =
        EAARLIO_EDB_HEADER_SIZE + EAARLIO_EDB_RECORD_SIZE * edb->record_count;

    err = eaarlio_edb_write_header(stream, &header);
    if(err != EAARLIO_SUCCESS)
        return err;

    if(edb->record_count > 0) {
        err = eaarlio_edb_write_records(stream, &header, edb->records);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    if(edb->file_count > 0) {
        err = eaarlio_edb_write_filenames(
            stream, &header, (char const *const *)edb->files);
        if(err != EAARLIO_SUCCESS)
            return err;
    }

    return EAARLIO_SUCCESS;
}
