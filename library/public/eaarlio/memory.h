#ifndef EAARLIO_MEMORY_H
#define EAARLIO_MEMORY_H

/**
 * @file
 * @brief Memory handler
 *
 * This header provides a means by which to override how the library handles
 * its memory allocation needs. All functions that perform memory allocation or
 * deallocation accept an ::eaarlio_memory pointer. When not provided (i.e.,
 * when @c NULL), the C standard library (stdlib.h) is used.
 *
 * If a function does not have an ::eaarlio_memory parameter, then it does
 * not perform any memory allocation or deallocation.
 */

#include <stddef.h>

/**
 * Memory handler structure
 *
 * This structure is designed to closely match the C standard library
 * (stdlib.h) memory functions. The default memory handler (if you provide @c
 * NULL as an argument to any function that asks for a memory handler) passes
 * its arguments through to the underlying standard library functions of the
 * same names.
 *
 * The @c opaque field can be used for whatever additional information the
 * caller needs for its internal handling of function calls. If you do not need
 * it, you can set it to @c NULL.
 *
 * Here's a simple illustration of how the memory handler is used within a
 * function:
 *
 *      function example(struct eaarlio_memory *memory)
 *      {
 *          // Allocate 10 bytes
 *          void *data = memory->malloc(memory, 10);
 *      }
 */
struct eaarlio_memory {
    /**
     * Allocate the specified number of bytes
     *
     * This should have the same behavior as defined for the standard library
     * function malloc.
     *
     * @param[in] self Pointer to memory handler
     * @param[in] size Bytes to allocate
     *
     * @returns On success, this should return a pointer to the newly allocated
     *      memory. On failure, this should return @c NULL.
     */
    void *(*malloc)(struct eaarlio_memory *self, size_t size);
    /**
     * Releases the specified block of memory back to the system
     *
     * This should have the same behavior as defined for the standard library
     * function free.
     *
     * @param[in] self Pointer to memory handler
     * @param[in] ptr Pointer to block of memory to release
     */
    void (*free)(struct eaarlio_memory *self, void *ptr);

    /**
     * Increases or decreases the size of the specified block of memory
     *
     * This should have the same behavior as defined for the standard library
     * function realloc.
     *
     * @param[in] self Pointer to memory handler
     * @param[in] ptr Pointer to block of memory to resize
     * @param[in] size Bytes to be allocated in the resized block of memory
     *
     * @returns On success, this should return a pointer to the resulting block
     *      of memory. On failure, this should return @c NULL.
     */
    void *(*realloc)(struct eaarlio_memory *self, void *ptr, size_t size);

    /**
     * Allocates the specified number of bytes and initializes them to zero
     *
     * This should have the same behavior as defined for the standard library
     * function calloc.
     *
     * @param[in] self Pointer to memory handler
     * @param[in] nmemb Number of elements in array to allocate
     * @param[in] size Size of each element in the array
     *
     * @returns On success, this should return a pointer to the newly allocated
     *      memory. On failure, this should return @c NULL.
     */
    void *(*calloc)(struct eaarlio_memory *self, size_t nmemb, size_t size);

    /**
     * Internal opaque pointer
     *
     * This is available for use if needed by a memory handler implementation
     * for tracking state or data.
     */
    void *opaque;
};

/**
 * Empty eaarlio_memory value
 *
 * All pointers will be @c NULL.
 */
#define eaarlio_memory_empty()                                                 \
    (struct eaarlio_memory)                                                    \
    {                                                                          \
        NULL, NULL, NULL, NULL, NULL                                           \
    }

#endif
