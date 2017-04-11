#include "eaarlio/memory_support.h"
#include <stdlib.h>

int eaarlio_memory_valid(struct eaarlio_memory const *memory)
{
    return memory && memory->malloc && memory->free && memory->realloc
        && memory->calloc;
}
