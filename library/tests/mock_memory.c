#include "mock_memory.h"
#include <stdlib.h>

static void *_mock_malloc(struct eaarlio_memory *self, size_t size)
{
    struct mock_memory *mock = (struct mock_memory *)self->opaque;
    int i;

    if(mock->ptrs_used == mock->ptrs_count) {
        return NULL;
    }

    i = mock->ptrs_used++;

    mock->ptrs[i] = malloc(size);
    return mock->ptrs[i];
}

static void _mock_free(struct eaarlio_memory *self, void *ptr)
{
    struct mock_memory *mock = (struct mock_memory *)self->opaque;
    int i;

    for(i = 0; i < mock->ptrs_used; i++) {
        if(mock->ptrs[i] == ptr) {
            free(ptr);
            mock->ptrs[i] = NULL;
            return;
        }
    }
}

static void *_mock_realloc(struct eaarlio_memory *self, void *ptr, size_t size)
{
    struct mock_memory *mock = (struct mock_memory *)self->opaque;
    int i;

    for(i = 0; i < mock->ptrs_used; i++) {
        if(mock->ptrs[i] == ptr) {
            break;
        }
    }

    if(i == mock->ptrs_used)
        return NULL;

    mock->ptrs[i] = realloc(mock->ptrs[i], size);
    return mock->ptrs[i];
}

static void *_mock_calloc(struct eaarlio_memory *self,
    size_t nmemb,
    size_t size)
{
    struct mock_memory *mock = (struct mock_memory *)self->opaque;
    int i;

    if(mock->ptrs_used == mock->ptrs_count) {
        return NULL;
    }

    i = mock->ptrs_used++;

    mock->ptrs[i] = calloc(nmemb, size);
    return mock->ptrs[i];
}

static void mock_memory_init(struct mock_memory *mock, int ptrs_count)
{
    int i;

    mock->ptrs_count = ptrs_count;
    mock->ptrs_used = 0;

    if(ptrs_count == 0) {
        mock->ptrs = NULL;
        return;
    }

    mock->ptrs = calloc(ptrs_count, sizeof(void *));
    for(i = 0; i < ptrs_count; i++)
        mock->ptrs[i] = NULL;
}

void mock_memory_new(struct eaarlio_memory *memory,
    struct mock_memory *mock,
    int ptrs_count)
{
    memory->malloc = &_mock_malloc;
    memory->free = &_mock_free;
    memory->realloc = &_mock_realloc;
    memory->calloc = &_mock_calloc;
    memory->opaque = mock;

    mock_memory_init(mock, ptrs_count);
}

static void mock_memory_free(struct mock_memory *mock)
{
    int i;

    for(i = 0; i < mock->ptrs_used; i++) {
        if(mock->ptrs[i])
            free(mock->ptrs[i]);
    }
    if(mock->ptrs)
        free(mock->ptrs);

    mock->ptrs = NULL;
    mock->ptrs_count = 0;
    mock->ptrs_used = 0;
}

void mock_memory_destroy(struct eaarlio_memory *memory)
{
    struct mock_memory *mock = (struct mock_memory *)memory->opaque;
    mock_memory_free(mock);

    memory->malloc = NULL;
    memory->free = NULL;
    memory->realloc = NULL;
    memory->calloc = NULL;
    memory->opaque = NULL;
}

void mock_memory_reset(struct mock_memory *mock, int ptrs_count)
{
    mock_memory_free(mock);
    mock_memory_init(mock, ptrs_count);
}

int mock_memory_count_in_use(struct mock_memory *mock)
{
    int i;
    int in_use = 0;

    for(i = 0; i < mock->ptrs_used; i++) {
        if(mock->ptrs[i])
            in_use++;
    }

    return in_use;
}
