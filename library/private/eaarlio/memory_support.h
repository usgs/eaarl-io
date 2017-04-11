#ifndef EAARLIO_MEMORY_SUPPORT_H
#define EAARLIO_MEMORY_SUPPORT_H

/**
 * @file
 * @brief Support code for memory handler
 */

#include "eaarlio/memory.h"
#include <stddef.h>

/**
 * Default memory handler
 *
 * Used when @c NULL is given as a memory handler. Uses the standard library.
 */
extern struct eaarlio_memory eaarlio_memory_default;

/**
 * Check the validity of the given memory handler
 *
 * For a memory handler to be valid, all four of its function pointers
 * (@c malloc, @c free, @c realloc, and @c calloc) must be non-null.
 *
 * This is used internally by functions that accept a memory handler. An
 * invalid memory handler will result in a ::EAARLIO_MEMORY_INVALID error by
 * such functions.
 *
 * @param[in] memory Memory handler
 *
 * @retval 1 if the memory handler is valid
 * @retval 0 if the memory handler is not valid
 */
int eaarlio_memory_valid(struct eaarlio_memory const *memory);

#endif
