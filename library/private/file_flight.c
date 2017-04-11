#include "eaarlio/file.h"
#include "eaarlio/memory_support.h"

eaarlio_error eaarlio_file_flight(struct eaarlio_flight *flight,
    char const *edb_file,
    char const *tld_path,
    struct eaarlio_memory *memory)
{
    struct eaarlio_stream stream = eaarlio_stream_empty();
    eaarlio_error err = EAARLIO_SUCCESS;

    if(!flight)
        return EAARLIO_NULL;
    *flight = eaarlio_flight_empty();

    if(!edb_file)
        return EAARLIO_NULL;
    if(!tld_path)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    err = eaarlio_file_stream(&stream, edb_file, "r");
    if(err != EAARLIO_SUCCESS)
        goto exit;

    err = eaarlio_edb_read(&stream, &flight->edb, memory, 1, 1);
    if(err != EAARLIO_SUCCESS)
        goto exit;

    err = stream.close(&stream);
    if(err != EAARLIO_SUCCESS)
        goto exit;

    err = eaarlio_file_tld_opener(&flight->tld_opener, tld_path, memory);
    if(err != EAARLIO_SUCCESS)
        goto exit;

    err = eaarlio_flight_init(flight, memory);
    if(err != EAARLIO_SUCCESS)
        goto exit;

exit:
    if(stream.close)
        stream.close(&stream);
    return err;
}
