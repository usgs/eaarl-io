#include "eaarlio/raster.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/pulse.h"

eaarlio_error eaarlio_raster_free(struct eaarlio_raster *raster,
    struct eaarlio_memory *memory)
{
    eaarlio_error err;
    uint32_t i;

    if(!raster)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    if(!raster->pulse)
        return EAARLIO_SUCCESS;

    for(i = 0; i < raster->pulse_count; i++) {
        err = eaarlio_pulse_free(&raster->pulse[i], memory);
        if(err)
            return err;
    }

    memory->free(memory, raster->pulse);
    raster->pulse = NULL;

    return EAARLIO_SUCCESS;
}
