#include "eaarlio/pulse.h"
#include "eaarlio/memory_support.h"

eaarlio_error eaarlio_pulse_free(struct eaarlio_pulse *pulse,
    struct eaarlio_memory *memory)
{
    uint32_t i;

    if(!pulse)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    if(pulse->tx) {
        memory->free(memory, pulse->tx);
        pulse->tx = NULL;
    }

    for(i = 0; i < EAARLIO_MAX_RX_COUNT; i++) {
        if(pulse->rx[i]) {
            memory->free(memory, pulse->rx[i]);
            pulse->rx[i] = NULL;
        }
    }

    return EAARLIO_SUCCESS;
}
