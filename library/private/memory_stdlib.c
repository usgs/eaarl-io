#include "eaarlio/memory.h"
#include <stdlib.h>

static void *eaarlio_memory_malloc(struct eaarlio_memory *self, size_t size)
{
    (void)self;
    return malloc(size);
}

static void eaarlio_memory_free(struct eaarlio_memory *self, void *ptr)
{
    (void)self;
    free(ptr);
}

static void *eaarlio_memory_realloc(struct eaarlio_memory *self,
    void *ptr,
    size_t size)
{
    (void)self;
    return realloc(ptr, size);
}

static void *eaarlio_memory_calloc(struct eaarlio_memory *self,
    size_t nmemb,
    size_t size)
{
    (void)self;
    return calloc(nmemb, size);
}

struct eaarlio_memory eaarlio_memory_default = { eaarlio_memory_malloc,
    eaarlio_memory_free, eaarlio_memory_realloc, eaarlio_memory_calloc, NULL };
