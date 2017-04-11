#include "eaarlio/edb_internals.h"
#include "eaarlio/file.h"
#include "eaarlio/memory_support.h"
#include "eaarlio/misc_support.h"
#include "eaarlio/tld_opener.h"

/**
 * Internal structure for file-based tld_opener
 */
struct _eaarlio_file_tld_opener {
    /** Path to the TLD files */
    char *path;

    /** Length of the path
     *
     * This simply caches @c strlen(path) so that it doesn't have to be
     * recalculated every time.
     */
    size_t path_len;

    /** Memory handler */
    struct eaarlio_memory memory;
};

/**
 * Implementation for ::eaarlio_tld_opener::open_tld
 */
static eaarlio_error _eaarlio_file_tld_opener_open_tld(
    struct eaarlio_tld_opener *self,
    struct eaarlio_stream *stream,
    char const *tld_file)
{
    if(!self)
        return EAARLIO_NULL;
    if(!stream)
        return EAARLIO_NULL;
    if(!tld_file)
        return EAARLIO_NULL;

    eaarlio_error err;
    struct _eaarlio_file_tld_opener *internal =
        (struct _eaarlio_file_tld_opener *)self->opaque;
    struct eaarlio_memory *memory = &internal->memory;

    size_t len = eaarlio_strnlen(tld_file, EAARLIO_EDB_FILENAME_MAX_LENGTH + 1);
    if(len > EAARLIO_EDB_FILENAME_MAX_LENGTH)
        return EAARLIO_STRING_UNTERMINATED;
    len += internal->path_len + 2;

    char *path = memory->malloc(memory, len);
    if(!path)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    strcpy(path, internal->path);
    strcat(path, "/");
    strcat(path, tld_file);

    err = eaarlio_file_stream(stream, path, "r");
    memory->free(memory, path);
    return err;
}

/**
 * Implementation for ::eaarlio_tld_opener::close
 */
static eaarlio_error _eaarlio_file_tld_opener_close(
    struct eaarlio_tld_opener *self)
{
    if(!self)
        return EAARLIO_NULL;

    struct _eaarlio_file_tld_opener *internal =
        (struct _eaarlio_file_tld_opener *)self->opaque;
    struct eaarlio_memory *memory = &internal->memory;

    memory->free(memory, internal->path);
    memory->free(memory, self->opaque);
    self->opaque = NULL;

    return EAARLIO_SUCCESS;
}

eaarlio_error eaarlio_file_tld_opener(struct eaarlio_tld_opener *opener,
    char const *path,
    struct eaarlio_memory *memory)
{
    struct _eaarlio_file_tld_opener *internal;
    size_t len;

    if(!opener)
        return EAARLIO_NULL;
    opener->opaque = NULL;
    opener->open_tld = NULL;
    opener->close = NULL;

    if(!path)
        return EAARLIO_NULL;

    if(!memory) {
        memory = &eaarlio_memory_default;
    } else if(!eaarlio_memory_valid(memory)) {
        return EAARLIO_MEMORY_INVALID;
    }

    len = eaarlio_strnlen(path, PATH_MAX);
    if(len == PATH_MAX) {
        return EAARLIO_STRING_UNTERMINATED;
    }

    internal = memory->malloc(memory, sizeof(struct _eaarlio_file_tld_opener));
    if(!internal)
        return EAARLIO_MEMORY_ALLOC_FAIL;

    internal->memory = *memory;
    internal->path_len = len;
    internal->path = memory->malloc(memory, internal->path_len + 1);
    if(!internal->path) {
        memory->free(memory, internal);
        return EAARLIO_MEMORY_ALLOC_FAIL;
    }

    strcpy(internal->path, path);

    opener->opaque = internal;
    opener->open_tld = _eaarlio_file_tld_opener_open_tld;
    opener->close = _eaarlio_file_tld_opener_close;

    return EAARLIO_SUCCESS;
}
