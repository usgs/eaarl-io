#ifndef MOCK_MEMORY_H
#define MOCK_MEMORY_H

#include "eaarlio/error.h"
#include "eaarlio/memory.h"
#include <assert.h>
#include <stdlib.h>

/**
 * Support for a mock memory
 */
struct mock_memory {
    /** An array of pointers to the memory we've allocated */
    void **ptrs;

    /** How many pointers are there in mock_memory::ptrs ? */
    int ptrs_count;

    /** How many pointers in mock_memory::ptrs have we used? */
    int ptrs_used;
};

/**
 * Initialize mock and configure memory to be mocked
 */
void mock_memory_new(struct eaarlio_memory *memory,
    struct mock_memory *mock,
    int ptrs_count);

/**
 * Release allocated memory and detach mock from memory
 */
void mock_memory_destroy(struct eaarlio_memory *memory);

/**
 * Reset mock: reinitialize with a new ptrs_count and clear memory
 */
void mock_memory_reset(struct mock_memory *mock, int ptrs_count);

/**
 * How many ptrs are still in use? (not freed)
 */
int mock_memory_count_in_use(struct mock_memory *mock);

#endif
